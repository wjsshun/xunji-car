#ifndef	__BSP_TRACK_H__
#define __BSP_TRACK_H__

#include "ti_msp_dl_config.h"



/* ================================================================
 *  循迹传感器电平逻辑配置 — 改这一个数字即可
 *  ------------------------------------------------------------
 *  1 = 传感器检测到黑线时输出 LOW(0)
 *  0 = 传感器检测到黑线时输出 HIGH(1)
 *
 *  宏保证 L4~R4 统一为：0=检测到黑线, 1=未检测/白线
 *  循迹代码无需任何修改，切换 TRACK_ACTIVE_LOW 即可适配硬件
 * ================================================================ */
#define TRACK_ACTIVE_LOW  0

#if TRACK_ACTIVE_LOW
/*
 * 传感器 LOW=黑线：引脚=0 时检测到黑线
 * 不需要取反：读 0 → 宏=0（黑）, 读>0 → 宏=1（白）
 */
#define L4  ( (DL_GPIO_readPins(TRACK_S1_PORT, TRACK_S1_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S1_PORT, TRACK_S1_PIN) )
#define L3  ( (DL_GPIO_readPins(TRACK_S2_PORT, TRACK_S2_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S2_PORT, TRACK_S2_PIN) )
#define L2  ( (DL_GPIO_readPins(TRACK_S3_PORT, TRACK_S3_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S3_PORT, TRACK_S3_PIN) )
#define L1  ( (DL_GPIO_readPins(TRACK_S4_PORT, TRACK_S4_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S4_PORT, TRACK_S4_PIN) )
#define R1  ( (DL_GPIO_readPins(TRACK_S5_PORT, TRACK_S5_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S5_PORT, TRACK_S5_PIN) )
#define R2  ( (DL_GPIO_readPins(TRACK_S6_PORT, TRACK_S6_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S6_PORT, TRACK_S6_PIN) )
#define R3  ( (DL_GPIO_readPins(TRACK_S7_PORT, TRACK_S7_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S7_PORT, TRACK_S7_PIN) )
#define R4  ( (DL_GPIO_readPins(TRACK_S8_PORT, TRACK_S8_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S8_PORT, TRACK_S8_PIN) )
#else
/*
 * 传感器 HIGH=黑线：引脚>0 时检测到黑线
 * 需要取反：读>0 → 取反→ 0（黑）, 读 0 → 取反→ 1（白）
 */
#define L4  !( (DL_GPIO_readPins(TRACK_S1_PORT, TRACK_S1_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S1_PORT, TRACK_S1_PIN) )
#define L3  !( (DL_GPIO_readPins(TRACK_S2_PORT, TRACK_S2_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S2_PORT, TRACK_S2_PIN) )
#define L2  !( (DL_GPIO_readPins(TRACK_S3_PORT, TRACK_S3_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S3_PORT, TRACK_S3_PIN) )
#define L1  !( (DL_GPIO_readPins(TRACK_S4_PORT, TRACK_S4_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S4_PORT, TRACK_S4_PIN) )
#define R1  !( (DL_GPIO_readPins(TRACK_S5_PORT, TRACK_S5_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S5_PORT, TRACK_S5_PIN) )
#define R2  !( (DL_GPIO_readPins(TRACK_S6_PORT, TRACK_S6_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S6_PORT, TRACK_S6_PIN) )
#define R3  !( (DL_GPIO_readPins(TRACK_S7_PORT, TRACK_S7_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S7_PORT, TRACK_S7_PIN) )
#define R4  !( (DL_GPIO_readPins(TRACK_S8_PORT, TRACK_S8_PIN) > 0) ? 1 : DL_GPIO_readPins(TRACK_S8_PORT, TRACK_S8_PIN) )
#endif

extern uint8_t now_lap;
extern float new_error;   // 当前循迹误差 (调试显示用)

void track_init(void);
float  Get_Distance_CM(void);
int track_scan(void);      // 读8路传感器并更新new_error
int track_control(void);
void turn_left();
bool track_all_black(void);  // 检测8路传感器是否全黑 (停止条件)
#endif
