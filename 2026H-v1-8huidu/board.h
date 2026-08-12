#ifndef	__BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"
#include "string.h"
#include "math.h"
#include <stdbool.h>
extern volatile uint32_t system_tick;

#define MOTOR_STOP    0
#define MOTOR_SPEED   1
#define MOTOR_DIR  		2
#define MOTOR_TRACK  	3

/* 系统模式枚举 */
typedef enum {
    MODE_IDLE = 0,
    MODE_DISTANCE = 1,
    MODE_ONELAP = 2
} SystemMode;

/* 固定距离模式目标距离 (毫米, 仅代码中修改) */
#define TARGET_DISTANCE_MM  66000

/* 跑一圈模式停止条件: 编码器累计脉冲数 (实测一圈 ≈22200) */
#define ONELAP_ENCODER_TARGET  15400

/* 编码器距离换算: mm = pulses * 2058 / 130 (13脉冲/圈, 周长205.8mm) */
#define PULSES_TO_MM(pulses)  ((int32_t)((pulses) * 2058 / 130))

extern SystemMode current_mode;
extern bool button3_state;

extern volatile uint32_t g_systick_count;
void board_init(void);

void delay_us(unsigned long __us);
void delay_ms(unsigned long ms);
void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);

void uart0_send_char(char ch);
void uart0_send_string(char* str);



#endif
