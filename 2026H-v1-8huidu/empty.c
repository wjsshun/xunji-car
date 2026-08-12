
 /* 2026-07-29     2026H-v1   双模式状态机, 3按键, 编码器停止
 */
#include "ti_msp_dl_config.h"
#include "board.h"
#include "stdio.h"
#include "bsp_motor.h"
#include "bsp_track.h"
#include "bsp_motor_hallencoder.h"
#include "oled_menu.h"
#include "key_handler.h"
#include "bsp_oled.h"
#include <stdlib.h>


// 系统滴答定时器计数
volatile uint32_t system_tick = 0;

// 系统模式状态
SystemMode current_mode = MODE_IDLE;
bool button3_state = false;

// 运动计时 (秒)
volatile uint32_t run_time_seconds = 0;
static uint32_t run_start_tick = 0;

// 编码器启动基准 (Delta法: 避免电气噪声导致Reset后乱跳)
static int32_t base_left_pulses = 0;
static int32_t base_right_pulses = 0;

// 定义3个按键
Key keys[KEY_COUNT] = {
    {KEY_PIN_14_PORT, KEY_PIN_14_PIN, 0, 0, false, false, false, false, false, 0, 0, 0}, // 按键1 PB14 
    {KEY_PIN_7_PORT,  KEY_PIN_7_PIN,  0, 0, false, false, false, false, false, 0, 0, 0}, // 按键2 PA7
    {KEY_PIN_1_PORT,  KEY_PIN_1_PIN,  0, 0, false, false, false, false, false, 0, 0, 0}, // 按键3 PB1
};

// 系统滴答定时器中断处理函数
void SysTick_Handler(void) {
    system_tick++;
    g_systick_count++;
}

// 传感器显示刷新缓存
static uint32_t last_dbg = 0;
static uint8_t  dbg_valid = 0;
static uint8_t  dbg_last_bits = 0;
static float    dbg_last_err = 0;
static bool     dbg_last_b3 = false;

// 获取当前累计脉冲(左右平均)
static int32_t Get_AvgPulses(void) {
    return (abs(Encoder_Get_TotalLeftPulses()) + abs(Encoder_Get_TotalRightPulses())) / 2;
}


int main(void)
{
    SYSCFG_DL_init();
    board_init();

    SysTick_Config(32000000 / 1000);

    Key_Init(keys, 20, 800);
    Motor_Init();
    motor_init();
    track_init();

    OLED_MenuInit();

    while (1)
    {
        Key_Scan(keys);

        /* ================================================================
         *  按键处理
         * ================================================================ */

        // 按键1 (PB14): 启动固定距离模式
        if (Key_GetShortPress(&keys[0]))
        {
            if (current_mode == MODE_IDLE)
            {
                // 先记录当前编码器基准 (此时电机还未转)
                base_left_pulses  = Encoder_Get_TotalLeftPulses();
                base_right_pulses = Encoder_Get_TotalRightPulses();
                run_start_tick = system_tick;
                run_time_seconds = 0;
                current_mode = MODE_DISTANCE;
                OLED_UpdateModeLines();
            }
        }

        // 按键2 (PA7): 启动跑一圈模式
        if (Key_GetShortPress(&keys[1]))
        {
            if (current_mode == MODE_IDLE)
            {
                base_left_pulses  = Encoder_Get_TotalLeftPulses();
                base_right_pulses = Encoder_Get_TotalRightPulses();
                run_start_tick = system_tick;
                run_time_seconds = 0;
                current_mode = MODE_ONELAP;
                OLED_UpdateModeLines();
            }
        }

        // 按键3 (PB1): 切换ON/OFF
        if (Key_GetShortPress(&keys[2]))
        {
            button3_state = !button3_state;
        }

        /* ================================================================
         *  状态机执行
         * ================================================================ */
        switch (current_mode)
        {
        case MODE_IDLE:
            if (system_tick - last_dbg >= 200)
            {
                last_dbg = system_tick;
                track_scan();

                uint8_t bits[8] = {L4, L3, L2, L1, R1, R2, R3, R4};
                uint8_t packed = 0;
                for (uint8_t i = 0; i < 8; i++)
                    packed = (uint8_t)((packed << 1) | (bits[i] ? 1 : 0));

                if (!dbg_valid || packed != dbg_last_bits || new_error != dbg_last_err || button3_state != dbg_last_b3)
                {
                    dbg_valid = 1;
                    dbg_last_bits = packed;
                    dbg_last_err = new_error;
                    dbg_last_b3 = button3_state;

                    /* 行3 (y=24): TR:XXXXXXXX+NN, 6x8字体, x坐标精确计算 */
                    for (uint8_t i = 0; i < 8; i++)
                        OLED_ShowChar(18 + 6 * i, 24, bits[i] ? '1' : '0', 8, 1);
                    OLED_ShowChar(72, 24, new_error < 0 ? '-' : '+', 8, 1);
                    OLED_ShowNum(80, 24, (u32)((new_error < 0 ? -new_error : new_error) * 2), 3, 8, 1);
                    OLED_RefreshRegion(3, 18, 97);

                    /* 行5 (y=40): B3:ON/OFF */
                    OLED_ShowString(0, 40, (u8*)"B3:", 8, 1);
                    OLED_ShowString(24, 40, (u8*)(button3_state ? "ON " : "OFF"), 8, 1);
                    OLED_RefreshRegion(5, 24, 42);
                }
            }
            break;

        case MODE_DISTANCE:
            track_control();

            run_time_seconds = (system_tick - run_start_tick) / 1000;

            {
                int32_t avg = Get_AvgPulses();
                int32_t base_avg = (abs(base_left_pulses) + abs(base_right_pulses)) / 2;
                int32_t delta = avg - base_avg;  // 本次运行的实际脉冲增量

                // 距离换算: pulses/13 * 205.8mm = pulses * 2058 / 130
                int32_t dist_mm = delta * 2058 / 130;
                if (dist_mm < 0) dist_mm = 0;

                // 距离转显示 (m)
                int32_t dist_m = dist_mm / 1000;
                int32_t dist_cm = (dist_mm % 1000) / 10;  // 两位小数

                /* 行4 (y=32): 编码器+距离, 逐位精确写入 */
                {
                    static int32_t last_disp = -1;
                    static uint32_t last_tick = 0;
                    if (system_tick - last_tick >= 200)
                    {
                        last_tick = system_tick;
                        // 清除该行
                        OLED_ShowString(0, 32, (u8*)"                    ", 8, 1);
                        // "E:"
                        OLED_ShowString(0, 32, (u8*)"E:", 8, 1);
                        // 脉冲数 (最多5位)
                        OLED_ShowNum(12, 32, (u32)delta, 5, 8, 1);
                        // " D:"
                        OLED_ShowString(48, 32, (u8*)"D:", 8, 1);
                        // 距离 X.XX m
                        OLED_ShowNum(60, 32, (u32)dist_m, 2, 8, 1);
                        OLED_ShowChar(72, 32, '.', 8, 1);
                        OLED_ShowNum(78, 32, (u32)dist_cm, 2, 8, 1);
                        OLED_ShowChar(90, 32, 'm', 8, 1);
                        OLED_RefreshRegion(4, 0, 96);

                        last_disp = delta;
                    }
                }

                /* 行7 (y=56): 运行时间 */
                {
                    static uint32_t last_time = 0;
                    if (system_tick - last_time >= 500)
                    {
                        last_time = system_tick;
                        OLED_ShowString(0, 56, (u8*)"T:     s", 8, 1);
                        OLED_ShowNum(18, 56, run_time_seconds, 5, 8, 1);
                        OLED_RefreshRegion(7, 0, 60);
                    }
                }

                // 停止条件: Delta距离 >= 目标
                if (dist_mm >= TARGET_DISTANCE_MM)
                {
                    motor_stop();
                    current_mode = MODE_IDLE;
                    OLED_UpdateModeLines();
                }
            }
            break;

        case MODE_ONELAP:
            track_control();

            run_time_seconds = (system_tick - run_start_tick) / 1000;

            {
                int32_t avg = Get_AvgPulses();
                int32_t base_avg = (abs(base_left_pulses) + abs(base_right_pulses)) / 2;
                int32_t delta = avg - base_avg;

                // 距离转换
                int32_t dist_mm = delta * 2058 / 130;
                if (dist_mm < 0) dist_mm = 0;
                int32_t dist_m = dist_mm / 1000;
                int32_t dist_cm = (dist_mm % 1000) / 10;

                /* 行4 (y=32): 编码器+距离 */
                {
                    static uint32_t last_tick = 0;
                    if (system_tick - last_tick >= 200)
                    {
                        last_tick = system_tick;
                        OLED_ShowString(0, 32, (u8*)"                    ", 8, 1);
                        OLED_ShowString(0, 32, (u8*)"E:", 8, 1);
                        OLED_ShowNum(12, 32, (u32)delta, 5, 8, 1);
                        OLED_ShowString(48, 32, (u8*)"D:", 8, 1);
                        OLED_ShowNum(60, 32, (u32)dist_m, 2, 8, 1);
                        OLED_ShowChar(72, 32, '.', 8, 1);
                        OLED_ShowNum(78, 32, (u32)dist_cm, 2, 8, 1);
                        OLED_ShowChar(90, 32, 'm', 8, 1);
                        OLED_RefreshRegion(4, 0, 96);
                    }
                }

                /* 行7 (y=56): 运行时间 */
                {
                    static uint32_t last_time = 0;
                    if (system_tick - last_time >= 500)
                    {
                        last_time = system_tick;
                        OLED_ShowString(0, 56, (u8*)"T:     s", 8, 1);
                        OLED_ShowNum(18, 56, run_time_seconds, 5, 8, 1);
                        OLED_RefreshRegion(7, 0, 60);
                    }
                }

                // 停止条件: 编码器脉冲 >= 22200 (实测一圈)
                if (delta >= ONELAP_ENCODER_TARGET)
                {
                    motor_stop();
                    current_mode = MODE_IDLE;
                    OLED_UpdateModeLines();
                }
            }
            break;
        }
    }
}
