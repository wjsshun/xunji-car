/*
 * bsp_mpu6050.c - MPU6050 陀螺仪驱动 + Mahony AHRS 姿态解算
 *
 * 硬件: MSPM0G3507, 硬件I2C1 (PB2=SCL, PB3=SDA, 100kHz)
 * INT:  PA12 下降沿中断
 * 算法: Mahony AHRS 四元数融合 (Pitch/Roll)
 *       + 独立梯形积分 Yaw (6层漂移防护)
 *       + 启动静止校准 + 运行时自动追零
 *
 * 参考:
 *   jremington/MPU-6050-Fusion (Mahony 3D filter)
 *   Mahony et al., "Nonlinear Complementary Filters on SO(3)"
 */

#include "bsp_mpu6050.h"
#include "board.h"
#include <math.h>

/* ================================================================
 *  全局变量定义
 * ================================================================ */
volatile uint8_t mpu_data_flag = 0;     /* PA12中断置位 */
volatile float mpu_yaw   = 0.0f;        /* Yaw 角 (°), [-180, 180] */
volatile float mpu_pitch = 0.0f;        /* Pitch 角 (°) */
volatile float mpu_roll  = 0.0f;        /* Roll 角 (°) */
volatile float display_yaw = 0.0f;      /* 显示用 yaw (TIMER ISR同步) */

/* ---- Mahony 四元数 ---- */
float mahony_q0 = 1.0f, mahony_q1 = 0.0f;
float mahony_q2 = 0.0f, mahony_q3 = 0.0f;

/* ---- 陀螺仪零偏估计 (校准+追踪) ---- */
static float gyro_bias_x = 0.0f;
static float gyro_bias_y = 0.0f;
static float gyro_bias_z = 0.0f;

/* ---- 陀螺仪前帧值 (梯形积分用) ---- */
static float gyro_z_prev = 0.0f;

/* ================================================================
 *  I2C 等待完成
 * ================================================================ */
static void i2c_wait_done(void)
{
    while (DL_I2C_getControllerStatus(I2C_MPU6050_INST)
           & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_MPU6050_INST)
             & DL_I2C_CONTROLLER_STATUS_IDLE));
}

/* ================================================================
 *  MPU6050 寄存器写 (硬件 I2C1, 轮询模式)
 * ================================================================ */
static void MPU_Write(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    DL_I2C_fillControllerTXFIFO(I2C_MPU6050_INST, buf, 2);
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    i2c_wait_done();
}

/* ================================================================
 *  MPU6050 寄存器读 (两阶段: 写地址 → 重复START读数据)
 * ================================================================ */
static uint8_t MPU_Read(uint8_t reg)
{
    uint8_t val;
    /* 阶段1: 写寄存器地址 */
    DL_I2C_fillControllerTXFIFO(I2C_MPU6050_INST, &reg, 1);
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    i2c_wait_done();
    /* 阶段2: 读数据 (重复START) */
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, 1);
    i2c_wait_done();
    val = DL_I2C_receiveControllerData(I2C_MPU6050_INST);
    return val;
}

/* ================================================================
 *  MPU6050 读取全部传感器原始数据 (14字节, 寄存器 0x3B~0x48)
 *  返回: raw_accel[3], raw_gyro[3] (int16_t 原始值)
 * ================================================================ */
static int16_t raw_accel_x, raw_accel_y, raw_accel_z;
static int16_t raw_gyro_x,  raw_gyro_y,  raw_gyro_z;

static void MPU_GetRawData(void)
{
    uint8_t buf[14];
    uint8_t i;

    for (i = 0; i < 14; i++)
    {
        buf[i] = MPU_Read(MPU6050_ACCEL_XOUT_H + i);
    }

    /* 加速度计: 大端 → int16 */
    raw_accel_x = (int16_t)((buf[0] << 8) | buf[1]);
    raw_accel_y = (int16_t)((buf[2] << 8) | buf[3]);
    raw_accel_z = (int16_t)((buf[4] << 8) | buf[5]);

    /* 跳过温度: buf[6], buf[7] */

    /* 陀螺仪: 大端 → int16 */
    raw_gyro_x = (int16_t)((buf[8]  << 8) | buf[9]);
    raw_gyro_y = (int16_t)((buf[10] << 8) | buf[11]);
    raw_gyro_z = (int16_t)((buf[12] << 8) | buf[13]);
}

/* ================================================================
 *  快速反平方根 (Carmack, 2次Newton迭代)
 *  用于四元数归一化, 在 Cortex-M0+ 软浮点上比 1/sqrtf() 快 ~3x
 * ================================================================ */
static float invSqrt(float x)
{
    long i;
    float x2, y;
    x2 = x * 0.5f;
    y = x;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (x2 * y * y));   /* 1st Newton */
    y = y * (1.5f - (x2 * y * y));   /* 2nd Newton */
    return y;
}

/* ================================================================
 *  校准层1: 启动静止校准 — 采样500次求均值, 排除运动中样本
 *  传感器必须保持静止! 上电后自动执行, 耗时约1秒
 * ================================================================ */
static void CalibrateGyro(void)
{
    float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;
    uint16_t count = 0;

    /* 基于时间的校准: 连续采样 500ms, 不计阈值, I2C阻塞节奏自动限速 */
    uint32_t start = g_systick_count;  /* 1ms tick */
    while (g_systick_count - start < 300)
    {
        MPU_GetRawData();
        sum_x += raw_gyro_x / GYRO_SENSITIVITY;
        sum_y += raw_gyro_y / GYRO_SENSITIVITY;
        sum_z += raw_gyro_z / GYRO_SENSITIVITY;
        count++;
    }

    if (count > 10)
    {
        gyro_bias_x = sum_x / (float)count;
        gyro_bias_y = sum_y / (float)count;
        gyro_bias_z = sum_z / (float)count;
    }
}

/* ================================================================
 *  Mahony AHRS 四元数更新 (Pitch/Roll PI修正, Yaw无磁力计无法修正)
 *
 *  输入: gx,gy,gz = 已减零偏的陀螺角速度 (°/s)
 *        ax,ay,az = 归一化前的加速度 (g)
 *        dt = 采样间隔 (s)
 *  输出: mahony_q0~q3 更新后的四元数
 *
 *  原理: 用加速度计测量的重力方向纠正陀螺仪的漂移
 *        PI控制器: ω_corrected = ω_raw + Kp·error + Ki·∫error
 * ================================================================ */
static void MahonyAHRSupdate(float gx, float gy, float gz,
                             float ax, float ay, float az,
                             float dt)
{
    float norm, halfT;
    float vx, vy, vz;
    float ex, ey, ez;
    static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;

    halfT = dt * 0.5f;

    /* 1. 归一化加速度 → 测量重力方向 */
    norm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    /* 2. 从当前四元数估计重力方向 */
    /*    v_est = q* ⊗ g ⊗ q, g = (0, 0, 0, 1) */
    vx = 2.0f * (mahony_q1 * mahony_q3 - mahony_q0 * mahony_q2);
    vy = 2.0f * (mahony_q0 * mahony_q1 + mahony_q2 * mahony_q3);
    vz = mahony_q0 * mahony_q0 - mahony_q1 * mahony_q1
       - mahony_q2 * mahony_q2 + mahony_q3 * mahony_q3;

    /* 3. 叉积误差 = a_meas × v_est */
    /*    方向 = 旋转轴, 大小 = 角度误差的正弦 */
    ex = ay * vz - az * vy;
    ey = az * vx - ax * vz;
    ez = ax * vy - ay * vx;  /* 绕重力轴旋转: 误差≈0 */

    /* 4. PI 修正 (带积分限幅防止 windup) */
    exInt += ex * MAHONY_Ki;
    eyInt += ey * MAHONY_Ki;
    ezInt += ez * MAHONY_Ki;

    if (exInt >  MAHONY_INT_LIMIT) exInt =  MAHONY_INT_LIMIT;
    if (exInt < -MAHONY_INT_LIMIT) exInt = -MAHONY_INT_LIMIT;
    if (eyInt >  MAHONY_INT_LIMIT) eyInt =  MAHONY_INT_LIMIT;
    if (eyInt < -MAHONY_INT_LIMIT) eyInt = -MAHONY_INT_LIMIT;
    if (ezInt >  MAHONY_INT_LIMIT) ezInt =  MAHONY_INT_LIMIT;
    if (ezInt < -MAHONY_INT_LIMIT) ezInt = -MAHONY_INT_LIMIT;

    gx += MAHONY_Kp * ex + exInt;
    gy += MAHONY_Kp * ey + eyInt;
    gz += MAHONY_Kp * ez + ezInt;

    /* 5. 一阶龙格-库塔积分四元数 */
    mahony_q0 += (-mahony_q1 * gx - mahony_q2 * gy - mahony_q3 * gz) * halfT;
    mahony_q1 += ( mahony_q0 * gx + mahony_q2 * gz - mahony_q3 * gy) * halfT;
    mahony_q2 += ( mahony_q0 * gy - mahony_q1 * gz + mahony_q3 * gx) * halfT;
    mahony_q3 += ( mahony_q0 * gz + mahony_q1 * gy - mahony_q2 * gx) * halfT;

    /* 6. 归一化四元数 (防止数值漂移) */
    norm = invSqrt(mahony_q0 * mahony_q0 + mahony_q1 * mahony_q1
                 + mahony_q2 * mahony_q2 + mahony_q3 * mahony_q3);
    mahony_q0 *= norm;
    mahony_q1 *= norm;
    mahony_q2 *= norm;
    mahony_q3 *= norm;
}

/* ================================================================
 *  Yaw 梯形积分 (独立于 Mahony, 多重保护)
 *
 *  层2: 死区滤波 — |gz| < DEAD_ZONE_DPS → 0
 *  层3: 梯形积分 — 2阶精度, 优于欧拉1阶
 *  层5: 异常限幅 — 角速度截断 + 单帧增量截断
 *  层6: 范围包裹 — [-180, 180]
 * ================================================================ */
static void YawTrapezoidalUpdate(float gz, float dt)
{
    float abs_gz, delta;

    /* 死区滤波: 抑制量化噪声 */
    abs_gz = (gz >= 0) ? gz : -gz;
    if (abs_gz < DEAD_ZONE_DPS) gz = 0.0f;

    /* 角速度异常截断 */
    if (gz >  MAX_DPS) gz =  MAX_DPS;
    if (gz < -MAX_DPS) gz = -MAX_DPS;

    /* 梯形积分: Δyaw = (gz(t-1) + gz(t)) × dt / 2  (O(dt³) 精度) */
    delta = (gyro_z_prev + gz) * 0.5f * dt;
    gyro_z_prev = gz;

    /* 单帧增量异常保护 */
    float abs_delta = (delta >= 0) ? delta : -delta;
    if (abs_delta > MAX_DELTA_YAW)
    {
        delta = (delta > 0) ? MAX_DELTA_YAW : -MAX_DELTA_YAW;
    }

    mpu_yaw += delta;

    /* 范围包裹 [-180, 180] */
    if (mpu_yaw >= 180.0f)  mpu_yaw -= 360.0f;
    if (mpu_yaw <= -180.0f) mpu_yaw += 360.0f;
}

/* ================================================================
 *  漂移防护层4: 运行时静止检测 + 自动零偏追踪
 *
 *  检测静止: |加速度幅值 - 1g| < 阈值 且 各轴角速度 < 阈值
 *  追踪算法: 指数移动平均, α=0.001 (时间常数 ~1000帧 ≈ 10秒)
 * ================================================================ */
static void RuntimeBiasUpdate(void)
{
    /* 使用原始值(未减零偏)做静止检测和零偏追踪 */
    float gx_raw = raw_gyro_x / GYRO_SENSITIVITY;
    float gy_raw = raw_gyro_y / GYRO_SENSITIVITY;
    float gz_raw = raw_gyro_z / GYRO_SENSITIVITY;
    float ax_raw = raw_accel_x / ACCEL_SENSITIVITY;
    float ay_raw = raw_accel_y / ACCEL_SENSITIVITY;
    float az_raw = raw_accel_z / ACCEL_SENSITIVITY;

    float acc_mag = (float)sqrt((double)(ax_raw * ax_raw + ay_raw * ay_raw + az_raw * az_raw));
    float diff = (acc_mag >= 1.0f) ? (acc_mag - 1.0f) : (1.0f - acc_mag);

    float abs_gx = (gx_raw >= 0) ? gx_raw : -gx_raw;
    float abs_gy = (gy_raw >= 0) ? gy_raw : -gy_raw;
    float abs_gz = (gz_raw >= 0) ? gz_raw : -gz_raw;

    uint8_t stationary = (diff  < STATIONARY_ACC_TH)  &&
                         (abs_gx < STATIONARY_GYRO_TH) &&
                         (abs_gy < STATIONARY_GYRO_TH) &&
                         (abs_gz < STATIONARY_GYRO_TH);

    if (stationary)
    {
        /* 静止时用原始值追踪零偏: bias += α × (raw_reading - bias) */
        gyro_bias_x += BIAS_TRACK_ALPHA * (gx_raw - gyro_bias_x);
        gyro_bias_y += BIAS_TRACK_ALPHA * (gy_raw - gyro_bias_y);
        gyro_bias_z += BIAS_TRACK_ALPHA * (gz_raw - gyro_bias_z);
    }
}

/* ================================================================
 *  Euler 角提取 (从 Mahony 四元数)
 *  仅在需要显示时调用, 不参与滤波积分
 * ================================================================ */
static void ExtractEulerAngles(void)
{
    float sin_pitch, abs_sp;

    /* Roll: 绕X轴 */
    mpu_roll = (float)atan2((double)(2.0f * (mahony_q0 * mahony_q1 + mahony_q2 * mahony_q3)),
                             (double)(1.0f - 2.0f * (mahony_q1 * mahony_q1 + mahony_q2 * mahony_q2)))
               * 57.2957795f;  /* 180/π */

    /* Pitch: 绕Y轴 */
    sin_pitch = 2.0f * (mahony_q0 * mahony_q2 - mahony_q3 * mahony_q1);
    abs_sp = (sin_pitch >= 0) ? sin_pitch : -sin_pitch;
    if (abs_sp >= 1.0f)
    {
        mpu_pitch = (sin_pitch >= 0) ? 90.0f : -90.0f;
    }
    else
    {
        mpu_pitch = (float)asin((double)sin_pitch) * 57.2957795f;
    }

    /* Yaw 不从四元数提取 — 由 YawTrapezoidalUpdate 独立管理 */
    /* (如果从四元数提取: 会和梯形积分 yaw 不同步, 导致显示跳变) */
}

/* ================================================================
 *  MPU6050_Init — 完整初始化
 *
 *  流程: 关I2C中断 → 唤醒MPU6050 → 配量程/采样率/DLPF
 *        → INT配置 → 等待稳定 → 静止校准 → 初始化四元数
 *  注意: I2C1硬件 + PA12中断 + 时钟由 SysConfig 的 SYSCFG_DL_init() 管理
 * ================================================================ */
void MPU6050_Init(void)
{
    /* 0. 关闭 SysConfig 自动启用的 I2C 中断 (使用轮询模式, 避免中断卡死) */
    DL_I2C_disableInterrupt(I2C_MPU6050_INST,
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST |
        DL_I2C_INTERRUPT_CONTROLLER_NACK          |
        DL_I2C_INTERRUPT_CONTROLLER_RXFIFO_TRIGGER |
        DL_I2C_INTERRUPT_CONTROLLER_RX_DONE        |
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

    /* 2. 退出睡眠模式, 选择内部 8MHz 振荡器 */
    MPU_Write(MPU6050_PWR_MGMT_1, 0x01);

    /* 3. 采样率 = 8kHz / (79+1) = 100Hz (匹配 MAHONY_SAMPLE_DT = 10ms) */
    MPU_Write(MPU6050_SMPLRT_DIV, 79);

    /* 4. 陀螺仪: ±250 dps (最高分辨率, 显示用场景不需要大范围) */
    MPU_Write(MPU6050_GYRO_CONFIG, GYRO_FS_SEL);

    /* 5. 加速度计: ±2g */
    MPU_Write(MPU6050_ACCEL_CONFIG, ACCEL_FS_SEL);

    /* 6. DLPF = 3 (加速度 41Hz, 陀螺仪 42Hz 低通截止) */
    /*    减少高频振动噪声, 同时保证 100Hz 采样不混叠 */
    MPU_Write(MPU6050_CONFIG, 0x03);

    /* 7. INT 引脚: 低电平有效, 推挽输出 */
    MPU_Write(MPU6050_INT_PIN_CFG, 0x80);

    /* 8. 使能数据就绪 (DATA_RDY) 中断 */
    MPU_Write(MPU6050_INT_ENABLE, 0x01);

    /* 9. 等待传感器稳定 (DLPF=3 需 200ms 以上) */
    delay_ms(200);

    /* 10. 静止校准 — 传感器必须保持静止! */
    CalibrateGyro();

    /* 11. Mahony 四元数初始化为单位四元数 */
    mahony_q0 = 1.0f;
    mahony_q1 = 0.0f;
    mahony_q2 = 0.0f;
    mahony_q3 = 0.0f;

    /* 12. 陀螺仪前帧值初始化 */
    gyro_z_prev = 0.0f;
    /* SysConfig 已配置 PA12 为下降沿中断 (GYRO GPIO 组) */
}

/* ================================================================
 *  mpu6050_task — 主数据任务 (每次 MPU6050 INT 中断后调用)
 *
 *  流程: 读原始数据 → 减零偏 → Mahony AHRS → 梯形积分Yaw
 *        → 提取Euler → 静止追零
 *
 *  耗时: ~500us @ 32MHz Cortex-M0+ (软浮点)
 * ================================================================ */
void mpu6050_task(void)
{
    float gx, gy, gz;
    float ax, ay, az;

    /* 1. 读原始数据 */
    MPU_GetRawData();

    /* 2. 转换为物理量并减零偏 */
    gx = raw_gyro_x / GYRO_SENSITIVITY - gyro_bias_x;
    gy = raw_gyro_y / GYRO_SENSITIVITY - gyro_bias_y;
    gz = raw_gyro_z / GYRO_SENSITIVITY - gyro_bias_z;

    ax = raw_accel_x / ACCEL_SENSITIVITY;
    ay = raw_accel_y / ACCEL_SENSITIVITY;
    az = raw_accel_z / ACCEL_SENSITIVITY;

    /* 3. Mahony AHRS 四元数更新 (主要修正 Pitch/Roll) */
    MahonyAHRSupdate(gx, gy, gz, ax, ay, az, MAHONY_SAMPLE_DT);

    /* 4. 独立 Yaw 梯形积分 + 多重保护 */
    YawTrapezoidalUpdate(gz, MAHONY_SAMPLE_DT);

    /* 5. 提取 Euler 角 (Pitch/Roll 从四元数, Yaw 已由梯形积分更新) */
    ExtractEulerAngles();

    /* 6. 运行时静止检测 + 自动零偏追踪 (用原始值, 防反向破坏) */
    RuntimeBiasUpdate();
}

/* ================================================================
 *  zero_yaw — Yaw 归零 (用于建立参考方向)
 * ================================================================ */
void zero_yaw(void)
{
    mpu_yaw = 0.0f;
}
