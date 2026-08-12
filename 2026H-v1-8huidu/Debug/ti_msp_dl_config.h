/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_MOTOR */
#define PWM_MOTOR_INST                                                     TIMG7
#define PWM_MOTOR_INST_IRQHandler                               TIMG7_IRQHandler
#define PWM_MOTOR_INST_INT_IRQN                                 (TIMG7_INT_IRQn)
#define PWM_MOTOR_INST_CLK_FREQ                                         32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_C0_PORT                                             GPIOA
#define GPIO_PWM_MOTOR_C0_PIN                                     DL_GPIO_PIN_28
#define GPIO_PWM_MOTOR_C0_IOMUX                                   (IOMUX_PINCM3)
#define GPIO_PWM_MOTOR_C0_IOMUX_FUNC                  IOMUX_PINCM3_PF_TIMG7_CCP0
#define GPIO_PWM_MOTOR_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_C1_PORT                                             GPIOA
#define GPIO_PWM_MOTOR_C1_PIN                                     DL_GPIO_PIN_31
#define GPIO_PWM_MOTOR_C1_IOMUX                                   (IOMUX_PINCM6)
#define GPIO_PWM_MOTOR_C1_IOMUX_FUNC                  IOMUX_PINCM6_PF_TIMG7_CCP1
#define GPIO_PWM_MOTOR_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                         (39999U)




/* Defines for I2C_MPU6050 */
#define I2C_MPU6050_INST                                                    I2C1
#define I2C_MPU6050_INST_IRQHandler                              I2C1_IRQHandler
#define I2C_MPU6050_INST_INT_IRQN                                  I2C1_INT_IRQn
#define I2C_MPU6050_BUS_SPEED_HZ                                          100000
#define GPIO_I2C_MPU6050_SDA_PORT                                          GPIOB
#define GPIO_I2C_MPU6050_SDA_PIN                                   DL_GPIO_PIN_3
#define GPIO_I2C_MPU6050_IOMUX_SDA                               (IOMUX_PINCM16)
#define GPIO_I2C_MPU6050_IOMUX_SDA_FUNC                IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_I2C_MPU6050_SCL_PORT                                          GPIOB
#define GPIO_I2C_MPU6050_SCL_PIN                                   DL_GPIO_PIN_2
#define GPIO_I2C_MPU6050_IOMUX_SCL                               (IOMUX_PINCM15)
#define GPIO_I2C_MPU6050_IOMUX_SCL_FUNC                IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_0_DEBUG */
#define UART_0_DEBUG_INST                                                  UART0
#define UART_0_DEBUG_INST_FREQUENCY                                     32000000
#define UART_0_DEBUG_INST_IRQHandler                            UART0_IRQHandler
#define UART_0_DEBUG_INST_INT_IRQN                                UART0_INT_IRQn
#define GPIO_UART_0_DEBUG_RX_PORT                                          GPIOA
#define GPIO_UART_0_DEBUG_TX_PORT                                          GPIOA
#define GPIO_UART_0_DEBUG_RX_PIN                                  DL_GPIO_PIN_11
#define GPIO_UART_0_DEBUG_TX_PIN                                  DL_GPIO_PIN_10
#define GPIO_UART_0_DEBUG_IOMUX_RX                               (IOMUX_PINCM22)
#define GPIO_UART_0_DEBUG_IOMUX_TX                               (IOMUX_PINCM21)
#define GPIO_UART_0_DEBUG_IOMUX_RX_FUNC                IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_DEBUG_IOMUX_TX_FUNC                IOMUX_PINCM21_PF_UART0_TX
#define UART_0_DEBUG_BAUD_RATE                                          (115200)
#define UART_0_DEBUG_IBRD_32_MHZ_115200_BAUD                                (17)
#define UART_0_DEBUG_FBRD_32_MHZ_115200_BAUD                                (23)





/* Port definition for Pin Group LED1 */
#define LED1_PORT                                                        (GPIOB)

/* Defines for PIN_11: GPIOB.11 with pinCMx 28 on package pin 63 */
#define LED1_PIN_11_PIN                                         (DL_GPIO_PIN_11)
#define LED1_PIN_11_IOMUX                                        (IOMUX_PINCM28)
/* Defines for PIN_1: GPIOB.1 with pinCMx 13 on package pin 48 */
#define KEY_PIN_1_PORT                                                   (GPIOB)
// groups represented: ["ENCODER","KEY"]
// pins affected: ["E1A","E1B","E2B","E2A","PIN_1"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define KEY_PIN_1_IIDX                                       (DL_GPIO_IIDX_DIO1)
#define KEY_PIN_1_PIN                                            (DL_GPIO_PIN_1)
#define KEY_PIN_1_IOMUX                                          (IOMUX_PINCM13)
/* Defines for PIN_14: GPIOB.14 with pinCMx 31 on package pin 2 */
#define KEY_PIN_14_PORT                                                  (GPIOB)
#define KEY_PIN_14_PIN                                          (DL_GPIO_PIN_14)
#define KEY_PIN_14_IOMUX                                         (IOMUX_PINCM31)
/* Defines for PIN_7: GPIOA.7 with pinCMx 14 on package pin 49 */
#define KEY_PIN_7_PORT                                                   (GPIOA)
#define KEY_PIN_7_PIN                                            (DL_GPIO_PIN_7)
#define KEY_PIN_7_IOMUX                                          (IOMUX_PINCM14)
/* Defines for AIN1_A02: GPIOA.2 with pinCMx 7 on package pin 42 */
#define MOTOR_AIN1_A02_PORT                                              (GPIOA)
#define MOTOR_AIN1_A02_PIN                                       (DL_GPIO_PIN_2)
#define MOTOR_AIN1_A02_IOMUX                                      (IOMUX_PINCM7)
/* Defines for AIN2_B24: GPIOB.24 with pinCMx 52 on package pin 23 */
#define MOTOR_AIN2_B24_PORT                                              (GPIOB)
#define MOTOR_AIN2_B24_PIN                                      (DL_GPIO_PIN_24)
#define MOTOR_AIN2_B24_IOMUX                                     (IOMUX_PINCM52)
/* Defines for BIN1_B20: GPIOB.20 with pinCMx 48 on package pin 19 */
#define MOTOR_BIN1_B20_PORT                                              (GPIOB)
#define MOTOR_BIN1_B20_PIN                                      (DL_GPIO_PIN_20)
#define MOTOR_BIN1_B20_IOMUX                                     (IOMUX_PINCM48)
/* Defines for BIN2_B19: GPIOB.19 with pinCMx 45 on package pin 16 */
#define MOTOR_BIN2_B19_PORT                                              (GPIOB)
#define MOTOR_BIN2_B19_PIN                                      (DL_GPIO_PIN_19)
#define MOTOR_BIN2_B19_IOMUX                                     (IOMUX_PINCM45)
/* Defines for S1: GPIOA.27 with pinCMx 60 on package pin 31 */
#define TRACK_S1_PORT                                                    (GPIOA)
#define TRACK_S1_PIN                                            (DL_GPIO_PIN_27)
#define TRACK_S1_IOMUX                                           (IOMUX_PINCM60)
/* Defines for S2: GPIOA.25 with pinCMx 55 on package pin 26 */
#define TRACK_S2_PORT                                                    (GPIOA)
#define TRACK_S2_PIN                                            (DL_GPIO_PIN_25)
#define TRACK_S2_IOMUX                                           (IOMUX_PINCM55)
/* Defines for S3: GPIOB.25 with pinCMx 56 on package pin 27 */
#define TRACK_S3_PORT                                                    (GPIOB)
#define TRACK_S3_PIN                                            (DL_GPIO_PIN_25)
#define TRACK_S3_IOMUX                                           (IOMUX_PINCM56)
/* Defines for S4: GPIOA.14 with pinCMx 36 on package pin 7 */
#define TRACK_S4_PORT                                                    (GPIOA)
#define TRACK_S4_PIN                                            (DL_GPIO_PIN_14)
#define TRACK_S4_IOMUX                                           (IOMUX_PINCM36)
/* Defines for S5: GPIOA.16 with pinCMx 38 on package pin 9 */
#define TRACK_S5_PORT                                                    (GPIOA)
#define TRACK_S5_PIN                                            (DL_GPIO_PIN_16)
#define TRACK_S5_IOMUX                                           (IOMUX_PINCM38)
/* Defines for S6: GPIOB.17 with pinCMx 43 on package pin 14 */
#define TRACK_S6_PORT                                                    (GPIOB)
#define TRACK_S6_PIN                                            (DL_GPIO_PIN_17)
#define TRACK_S6_IOMUX                                           (IOMUX_PINCM43)
/* Defines for S7: GPIOB.21 with pinCMx 49 on package pin 20 */
#define TRACK_S7_PORT                                                    (GPIOB)
#define TRACK_S7_PIN                                            (DL_GPIO_PIN_21)
#define TRACK_S7_IOMUX                                           (IOMUX_PINCM49)
/* Defines for S8: GPIOA.30 with pinCMx 5 on package pin 37 */
#define TRACK_S8_PORT                                                    (GPIOA)
#define TRACK_S8_PIN                                            (DL_GPIO_PIN_30)
#define TRACK_S8_IOMUX                                            (IOMUX_PINCM5)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOA)

/* Defines for SCL: GPIOA.1 with pinCMx 2 on package pin 34 */
#define OLED_SCL_PIN                                             (DL_GPIO_PIN_1)
#define OLED_SCL_IOMUX                                            (IOMUX_PINCM2)
/* Defines for SDA: GPIOA.0 with pinCMx 1 on package pin 33 */
#define OLED_SDA_PIN                                             (DL_GPIO_PIN_0)
#define OLED_SDA_IOMUX                                            (IOMUX_PINCM1)
/* Port definition for Pin Group ENCODER */
#define ENCODER_PORT                                                     (GPIOB)

/* Defines for E1A: GPIOB.18 with pinCMx 44 on package pin 15 */
#define ENCODER_E1A_IIDX                                    (DL_GPIO_IIDX_DIO18)
#define ENCODER_E1A_PIN                                         (DL_GPIO_PIN_18)
#define ENCODER_E1A_IOMUX                                        (IOMUX_PINCM44)
/* Defines for E1B: GPIOB.6 with pinCMx 23 on package pin 58 */
#define ENCODER_E1B_IIDX                                     (DL_GPIO_IIDX_DIO6)
#define ENCODER_E1B_PIN                                          (DL_GPIO_PIN_6)
#define ENCODER_E1B_IOMUX                                        (IOMUX_PINCM23)
/* Defines for E2B: GPIOB.8 with pinCMx 25 on package pin 60 */
#define ENCODER_E2B_IIDX                                     (DL_GPIO_IIDX_DIO8)
#define ENCODER_E2B_PIN                                          (DL_GPIO_PIN_8)
#define ENCODER_E2B_IOMUX                                        (IOMUX_PINCM25)
/* Defines for E2A: GPIOB.7 with pinCMx 24 on package pin 59 */
#define ENCODER_E2A_IIDX                                     (DL_GPIO_IIDX_DIO7)
#define ENCODER_E2A_PIN                                          (DL_GPIO_PIN_7)
#define ENCODER_E2A_IOMUX                                        (IOMUX_PINCM24)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_MOTOR_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_I2C_MPU6050_init(void);
void SYSCFG_DL_UART_0_DEBUG_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
