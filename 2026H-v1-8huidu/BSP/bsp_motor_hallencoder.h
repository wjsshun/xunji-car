

#ifndef	__BSP_MOTOR_HALLENCODER_H__
#define __BSP_MOTOR_HALLENCODER_H__

#include "board.h"

// ��þ���ֵ
#define ABS(a)      (a>0 ? a:(-a))

typedef struct{
   int32_t Should_Get_Encoder_Count;   // ��Ҫ��õı���������
   int32_t Obtained_Get_Encoder_Count; // �õ��ı������ļ���
}Encoder;

// �����ۼƼ����������ⲿ�ɷ��ʣ�
extern volatile int32_t total_left_pulses;   // �������������ۼ�
extern volatile int32_t total_right_pulses;  // �������������ۼ�

/* Encoder cumulative reset function */
void Encoder_ResetTotals(void);

// ��ʼ���ۼƼ��������㣩
void Encoder_TotalCount_Init(void);

// ��ȡ���������ۼ�������
int32_t Encoder_Get_TotalLeftPulses(void);
int32_t Encoder_Get_TotalRightPulses(void);

void Motor_Init(void);
int Motor_Get_Encoder(int dir);
void Motor_Set_PWM(int pwma,int pwmb);
void Motor_Stop(void);

#endif