#include "bsp_motor.h"
#include "stdio.h"
#include "bsp_pid.h"
#include "bsp_key.h"

uint32_t gpio_interrup;


void motor_init(void)
{
	//编码器引脚外部中断
	NVIC_ClearPendingIRQ(GPIOB_INT_IRQn);
	NVIC_EnableIRQ(GPIOB_INT_IRQn);

	printf("Motor initialized successfully\r\n");
}

//左右电机速度控制，范围0-9999
void Set_PWM(int pwma,int pwmb)
{
	if(pwma>0) //左轮前进
	{
		DL_GPIO_setPins(MOTOR_AIN2_B24_PORT,MOTOR_AIN2_B24_PIN);
		DL_GPIO_clearPins(MOTOR_AIN1_A02_PORT,MOTOR_AIN1_A02_PIN);	
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(pwma),GPIO_PWM_MOTOR_C0_IDX);

	}
	else //左轮后退
	{
		DL_GPIO_setPins(MOTOR_AIN1_A02_PORT,MOTOR_AIN1_A02_PIN);
		DL_GPIO_clearPins(MOTOR_AIN2_B24_PORT,MOTOR_AIN2_B24_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(pwma),GPIO_PWM_MOTOR_C0_IDX);
	}
	if(pwmb>0)//右轮前进
	{
		DL_GPIO_setPins(MOTOR_BIN1_B20_PORT,MOTOR_BIN1_B20_PIN);
		DL_GPIO_clearPins(MOTOR_BIN2_B19_PORT,MOTOR_BIN2_B19_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(pwmb),GPIO_PWM_MOTOR_C1_IDX);
	}
  else//右轮后退
	{
		DL_GPIO_setPins(MOTOR_BIN2_B19_PORT,MOTOR_BIN2_B19_PIN);
		DL_GPIO_clearPins(MOTOR_BIN1_B20_PORT,MOTOR_BIN1_B20_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(pwmb),GPIO_PWM_MOTOR_C1_IDX);
	}		

}

void motor_stop()
{
		DL_GPIO_setPins(MOTOR_AIN1_A02_PORT,MOTOR_AIN1_A02_PIN);
		DL_GPIO_setPins(MOTOR_AIN2_B24_PORT,MOTOR_AIN2_B24_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,0,GPIO_PWM_MOTOR_C0_IDX);

		DL_GPIO_setPins(MOTOR_BIN2_B19_PORT,MOTOR_BIN2_B19_PIN);
		DL_GPIO_setPins(MOTOR_BIN1_B20_PORT,MOTOR_BIN1_B20_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,0,GPIO_PWM_MOTOR_C1_IDX);

}


