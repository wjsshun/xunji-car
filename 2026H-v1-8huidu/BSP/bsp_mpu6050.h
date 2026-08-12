/*
 * bsp_mpu6050.h - MPU6050 陀螺仪驱动 + Mahony AHRS 姿态解算
 *
 * 硬件: MSPM0G3507, 硬件I2C1 (PB2=SCL, PB3=SDA, 100kHz)
 * INT:  PA12 下降沿中断 (GPIOA 独立组, 不与编码器GPIOB冲突)
 * 算法: Mahony AHRS 四元数 (Pitch/Roll) + 独立梯形积分 (Yaw)
 *       6层漂移防护: 静止校准/死区/梯形积分/自动追零/异常限幅/包裹
 */
#ifndef __BSP_MPU6050_H__
#define __BSP_MPU6050_H__

#include "board.h"
#include <math.h>

/* ---- 硬件 I2C 实例定义 ---- */
#define I2C_MPU6050_INST    I2C1

/* ---- MPU6050 从机地址 (AD0 接地) ---- */
#define MPU6050_ADDR        0x68

/* ---- MPU6050 寄存器地址定义 ---- */
#define MPU6050_PWR_MGMT_1      0x6B    /* 电源管理1 */
#define MPU6050_PWR_MGMT_2      0x6C    /* 电源管理2 */
#define MPU6050_CONFIG          0x1A    /* DLPF配置 */
#define MPU6050_GYRO_CONFIG     0x1B    /* 陀螺仪量程配置 */
#define MPU6050_ACCEL_CONFIG    0x1C    /* 加速度计量程配置 */
#define MPU6050_SMPLRT_DIV      0x19    /* 采样率分频 */
#define MPU6050_INT_PIN_CFG     0x37    /* 中断引脚配置 */
#define MPU6050_INT_ENABLE      0x38    /* 中断使能 */
#define MPU6050_ACCEL_XOUT_H    0x3B    /* 加速度X高字节 */
#define MPU6050_ACCEL_XOUT_L    0x3C
#define MPU6050_ACCEL_YOUT_H    0x3D
#define MPU6050_ACCEL_YOUT_L    0x3E
#define MPU6050_ACCEL_ZOUT_H    0x3F
#define MPU6050_ACCEL_ZOUT_L    0x40
#define MPU6050_TEMP_OUT_H      0x41    /* 温度高字节 */
#define MPU6050_TEMP_OUT_L      0x42
#define MPU6050_GYRO_XOUT_H     0x43    /* 陀螺仪X高字节 */
#define MPU6050_GYRO_XOUT_L     0x44
#define MPU6050_GYRO_YOUT_H     0x45
#define MPU6050_GYRO_YOUT_L     0x46
#define MPU6050_GYRO_ZOUT_H     0x47
#define MPU6050_GYRO_ZOUT_L     0x48
#define MPU6050_WHO_AM_I        0x75    /* 设备ID (应返回0x68) */
#define MPU6050_SIGNAL_PATH_RESET 0x68  /* 信号路径复位 */

/* ---- 中断引脚: PA12 (下降沿), GPIOA 独立中断组 ---- */
#define MPU6050_INT_PORT    (GPIOA)
#define MPU6050_INT_PIN     DL_GPIO_PIN_12

/* ---- MPU6050 传感器量程 ---- */
#define GYRO_FS_SEL         0x00    /* ±250 dps */
#define GYRO_SENSITIVITY    131.0f  /* LSB per °/s */
#define ACCEL_FS_SEL        0x00    /* ±2g */
#define ACCEL_SENSITIVITY   16384.0f /* LSB per g */

/* ---- Mahony AHRS 参数 ---- */
#define MAHONY_Kp           2.0f    /* 比例增益: 越大收敛越快, 太大震荡 */
#define MAHONY_Ki           0.005f  /* 积分增益: 消除稳态偏置 */
#define MAHONY_SAMPLE_DT    0.01f   /* 采样周期 10ms (=100Hz) */
#define MAHONY_INT_LIMIT    0.3f    /* PI 积分限幅, 防止 windup */

/* ---- 校准与漂移防护参数 ---- */
#define CALIB_SAMPLES       200     /* 启动静止校准采样数 (减少热漂移影响) */
#define DEAD_ZONE_DPS       0.25f   /* 角速度死区 (°/s), 低于此值视为0 */
#define MAX_DPS             500.0f  /* 角速度异常上限 (°/s) */
#define MAX_DELTA_YAW       5.0f    /* 单帧 yaw 增量上限 (°) */
#define STATIONARY_ACC_TH   0.02f   /* 静止检测: |acc_mag - 1g| < 阈值 */
#define STATIONARY_GYRO_TH  0.5f    /* 静止检测: 各轴角速度 < 阈值 (°/s) */
#define BIAS_TRACK_ALPHA    0.001f  /* 零偏追踪系数 (时间常数 ~1000次) */
#define BIAS_INIT_THRESH    10.0f   /* 初始化时: 角速度>此值 = 传感器在运动 (放宽抗振动) */

/* ---- 全局变量声明 ---- */
extern volatile uint8_t mpu_data_flag;  /* PA12中断置位, 主循环读取并清零 */
extern volatile float mpu_yaw;          /* Yaw角 (°), 范围[-180,180] */
extern volatile float mpu_pitch;        /* Pitch角 (°) */
extern volatile float mpu_roll;         /* Roll角 (°) */
extern volatile float display_yaw;      /* 显示用 yaw 副本 (TIMER ISR 同步) */

/* ---- 函数声明 ---- */
void MPU6050_Init(void);        /* 初始化: I2C1 + MPU6050配置 + 校准 + PB1中断 */
void mpu6050_task(void);        /* 主任务: 读数据 + Mahony + 梯形积分 + 追零 */
void zero_yaw(void);            /* Yaw 归零 */

#endif /* __BSP_MPU6050_H__ */
