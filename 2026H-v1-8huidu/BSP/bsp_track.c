#include "bsp_track.h"
#include "board.h"
#include "stdio.h"
#include "bsp_motor.h"
#include "oled_menu.h"
#include "bsp_motor_hallencoder.h"
#include <stdlib.h>  // 包含 abs() 函数的声明
#include "bsp_oled.h"



//float Kp = 800, Ki=0, Kd =5;//PID参数
float Kp = 500, Ki=0, Kd =0;//PID参数
float P = 0, I = 0, D = 0, PID_value = 0;
float new_error = 0, previous_error = 0;
static int initial_motor_speed = 4000;//基础速度
uint8_t now_lap=0;

// 硬件参数（需根据实际小车填写）
// #define WHEEL_CIRCUMFERENCE  15.08f  // 轮子周长（单位：cm，例如直径4.15cm的轮子，周长≈π×4.15≈13cm）
// #define ENCODER_RESOLUTION   13    // 编码器分辨率（每圈脉冲数，例如13线编码器）
#define WHEEL_CIRCUMFERENCE  20.58f  // 轮子周长（单位：cm，例如直径4.15cm的轮子，周长≈π×4.15≈13cm）
#define ENCODER_RESOLUTION   13    // 编码器分辨率（每圈脉冲数，例如13线编码器）


// 计算距离（单位：cm）
float Get_Distance_CM(void) {
    // 取左右轮平均脉冲数（减少误差）
    int32_t avg_pulses = (abs(total_left_pulses) + abs(total_right_pulses)) / 2;
    // 距离 = （总脉冲数 / 每圈脉冲数）× 轮子周长
    return (avg_pulses * 1.0f / ENCODER_RESOLUTION) * WHEEL_CIRCUMFERENCE;
}



// 检测8路传感器是否全部检测到黑线 (用于跑一圈停止条件)
bool track_all_black(void) {
    return (L4 == 0 && L3 == 0 && L2 == 0 && L1 == 0 &&
            R1 == 0 && R2 == 0 && R3 == 0 && R4 == 0);
}

void track_init(void)
{
	printf("Track initialized successfully\r\n");
}



// 8路传感器从左到右排列：L4（最左）、L3、L2、L1、R1、R2、R3、R4（最右）
// 0 = 检测到黑线，1 = 检测到白线

int track_scan(void)
{
    // 打印8路传感器状态（调试用，顺序：L4,L3,L2,L1,R1,R2,R3,R4）
    // printf("%d,%d,%d,%d,%d,%d,%d,%d\n", L4, L3, L2, L1, R1, R2, R3, R4);

    // 左偏状态（L系列传感器检测到黑线）
    if (!L4 && L3 && L2 && L1 && R1 && R2 && R3 && R4)       // 仅L4（最左）检测到黑线
        new_error = 2;
    else if (!L4 && !L3 && L2 && L1 && R1 && R2 && R3 && R4)  // L4、L3检测到黑线
        new_error = 2;
    else if (L4 && !L3 && L2 && L1 && R1 && R2 && R3 && R4)   // 仅L3检测到黑线
        new_error = 2;
    else if (L4 && !L3 && !L2 && L1 && R1 && R2 && R3 && R4)  // L3、L2检测到黑线
        new_error = 2;
    else if (L4 && L3 && !L2 && L1 && R1 && R2 && R3 && R4)   // 仅L2检测到黑线
        new_error = 1;
    else if (L4 && L3 && !L2 && !L1 && R1 && R2 && R3 && R4)  // L2、L1检测到黑线
        new_error = 0.5;
		
    else if (L4 && L3 && L2 && !L1 && R1 && R2 && R3 && R4)   // 仅L1（左中）检测到黑线
        new_error = 0.5;
    // 中间状态（中线附近，L1和R1附近）
    else if (L4 && L3 && L2 && !L1 && !R1 && R2 && R3 && R4)  // L1、R1（中线）检测到黑线
        new_error = 0;
    else if (L4 && L3 && L2 && L1 && !R1 && R2 && R3 && R4)   // 仅R1（右中）检测到黑线
        new_error = -0.5;

    // 右偏状态（R系列传感器检测到黑线）
    else if (L4 && L3 && L2 && L1 && !R1 && !R2 && R3 && R4)  // R1、R2检测到黑线
        new_error = -0.5;
    else if (L4 && L3 && L2 && L1 && R1 && !R2 && R3 && R4)   // 仅R2检测到黑线
        new_error = -1;
    else if (L4 && L3 && L2 && L1 && R1 && !R2 && !R3 && R4)  // R2、R3检测到黑线
        new_error = -2;
    else if (L4 && L3 && L2 && L1 && R1 && R2 && !R3 && R4)   // 仅R3检测到黑线
        new_error = -2;
    else if (L4 && L3 && L2 && L1 && R1 && R2 && !R3 && !R4)  // R3、R4检测到黑线
        new_error = -2;
    else if (L4 && L3 && L2 && L1 && R1 && R2 && R3 && !R4)   // 仅R4（最右）检测到黑线
        new_error = -2;


    else if (!L4 && !L3 && !L2 && !L1 && !R1 && !R2 && !R3 && !R4)
        new_error = 0;  // 建议改为停止指令
			
    return 0;
}


int track_pid(void)
{  
	P=new_error;	        //当前误差
	I=I+new_error;	        //误差累加
	D=new_error-previous_error;	//当前误差与之前误差的误差
	
	PID_value=(Kp*P)+(Ki*I)+(Kd*D);
	
	previous_error=new_error;//更新之前误差
//	printf("%3f\n",PID_value);
	return PID_value; //返回速度控制值
}

//电机动作
void  motorsWrite(int speedL,int speedR)
{
	if(speedR > 0) 
	{
		//右轮前进
		DL_GPIO_setPins(MOTOR_BIN1_B20_PORT,MOTOR_BIN1_B20_PIN);
		DL_GPIO_clearPins(MOTOR_BIN2_B19_PORT,MOTOR_BIN2_B19_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(speedR),GPIO_PWM_MOTOR_C1_IDX);
	}
	else	
	{
		//右轮后退
		DL_GPIO_setPins(MOTOR_BIN2_B19_PORT,MOTOR_BIN2_B19_PIN);
		DL_GPIO_clearPins(MOTOR_BIN1_B20_PORT,MOTOR_BIN1_B20_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(speedR),GPIO_PWM_MOTOR_C1_IDX);
	}
	
	if(speedL > 0)
	{
		//左轮前进
		DL_GPIO_setPins(MOTOR_AIN2_B24_PORT,MOTOR_AIN2_B24_PIN);
		DL_GPIO_clearPins(MOTOR_AIN1_A02_PORT,MOTOR_AIN1_A02_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(speedL),GPIO_PWM_MOTOR_C0_IDX);
	}
	else
	{
		//左轮后退
		DL_GPIO_setPins(MOTOR_AIN1_A02_PORT,MOTOR_AIN1_A02_PIN);
		DL_GPIO_clearPins(MOTOR_AIN2_B24_PORT,MOTOR_AIN2_B24_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST,ABS(speedL),GPIO_PWM_MOTOR_C0_IDX);
	}
}
void track_Set_PWM(int pwm_value)
{
	//基础速度+PID值
	int left_motor_speed = initial_motor_speed-pwm_value;
	int right_motor_speed = initial_motor_speed+pwm_value;
	
	
	if(left_motor_speed>9999) left_motor_speed=9999;
	else if(left_motor_speed<-9999) left_motor_speed=-9999;
	if(right_motor_speed>9999) right_motor_speed=9999;
	else if(right_motor_speed<-9999) right_motor_speed=-9999;
	
	motorsWrite(-left_motor_speed,right_motor_speed);
}

		uint8_t flag = 0;



int track_control(void)
{
    track_scan();
    track_Set_PWM(track_pid());
    
    if(L4 && L3 && L2 && L1 && R1 && R2 && R3 && R4) 
    {
        delay_ms(500);
        turn_left();    
			  DL_GPIO_setPins(LED1_PORT, LED1_PIN_11_PIN);

        }

    return 0;
}


//}
void turn_left()
{
    // 1. 开始转弯（设置转向速度）
    motorsWrite(-1675, 0);//改成右转，左转是  motorsWrite(0, 1675);
    
    // 2. 等待转弯完成（仅判断最终回到轨迹的状态）
    // （根据实际调试，选择一个能代表转弯完成的传感器状态）
    while(1)
    {
        track_scan();  // 实时更新传感器状态
        // 例如：当传感器回到中线（new_error == 0）时，视为转弯完成
        if(new_error == 0) 
					break;
				
           
    }
//    delay_ms(5);
    // 3. 仅在一次完整转弯后，now_lap加1
    now_lap++;
}
