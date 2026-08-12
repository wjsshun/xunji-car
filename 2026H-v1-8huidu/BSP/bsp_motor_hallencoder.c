
#include "bsp_motor_hallencoder.h"

static volatile Encoder Encoder_A;
static volatile Encoder Encoder_B;

// �������ۼƼ���������ȫ�֣���¼����������
volatile int32_t total_left_pulses = 0;
volatile int32_t total_right_pulses = 0;


// ��������ʼ���ۼƼ��������㣩
// Encoder cumulative reset (enter fixed distance mode)
void Encoder_ResetTotals(void) {
    total_left_pulses = 0;
    total_right_pulses = 0;
}
void Encoder_TotalCount_Init(void) {
    total_left_pulses = 0;
    total_right_pulses = 0;
    // ͬʱ�����������ǰֵ����ѡ����������
    Encoder_A.Should_Get_Encoder_Count = 0;
    Encoder_A.Obtained_Get_Encoder_Count = 0;
    Encoder_B.Should_Get_Encoder_Count = 0;
    Encoder_B.Obtained_Get_Encoder_Count = 0;
}

// ��������ȡ�������ۼ�������
int32_t Encoder_Get_TotalLeftPulses(void) {
    return total_left_pulses;
}

// ��������ȡ�������ۼ�������
int32_t Encoder_Get_TotalRightPulses(void) {
    return total_right_pulses;
}


/******************************************************************
 * �� �� �� �ƣ�Motor_Init
 * �� �� ˵ ����������������ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LCKFB
 * ��       ע����
******************************************************************/
void Motor_Init(void)
{
	//�����������ⲿ�ж�
	NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
	NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);

    //��ʱ���ж�
	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);


}




/******************************************************************
 * �� �� �� �ƣ�Motor_Get_Encoder
 * �� �� ˵ ������ȡ��������ֵ
 * �� �� �� �Σ�dir=0��ȡ���ֱ�����ֵ  dir=1��ȡ���ֱ�����ֵ
 * �� �� �� �أ����ض�Ӧ�ı�����ֵ
 * ��       �ߣ�LCKFB
 * ��       ע����
******************************************************************/
int Motor_Get_Encoder(int dir)
{
	if( !dir )
		return Encoder_A.Obtained_Get_Encoder_Count;

	return Encoder_B.Obtained_Get_Encoder_Count;
}


/*******************************************************
�������ܣ��ⲿ�ж�ģ��������ź�
��ں�������
����  ֵ����
***********************************************************/
void GROUP1_IRQHandler(void)
{
    uint32_t gpio_interrup = 0;

    /* ---- GPIOA: KEY_PIN_7 (PA7) interrupt ---- */
    gpio_interrup = DL_GPIO_getEnabledInterruptStatus(GPIOA, KEY_PIN_7_PIN);
    if (gpio_interrup)
        DL_GPIO_clearInterruptStatus(GPIOA, gpio_interrup);

    //��ȡGPIOB�����ж�״̬ (ֻ����������, ������MPU6050 PB1�ж�)
    gpio_interrup = DL_GPIO_getEnabledInterruptStatus(ENCODER_PORT, KEY_PIN_1_PIN|ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);

    /* ---- ������ A (PB18, PB6) ---- */
	if((gpio_interrup & ENCODER_E1A_PIN)==ENCODER_E1A_PIN)
	{
		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E1B_PIN))
		{
			Encoder_A.Should_Get_Encoder_Count--;
		}
		else
		{
			Encoder_A.Should_Get_Encoder_Count++;
		}
	}
	else if((gpio_interrup & ENCODER_E1B_PIN)==ENCODER_E1B_PIN)
	{
		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E1A_PIN))
		{
			Encoder_A.Should_Get_Encoder_Count++;
		}
		else
		{
			Encoder_A.Should_Get_Encoder_Count--;
		}
	}

	// encoderB
	if((gpio_interrup & ENCODER_E2A_PIN)==ENCODER_E2A_PIN)
	{
		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E2B_PIN))
		{
			Encoder_B.Should_Get_Encoder_Count--;
		}
		else
		{
			Encoder_B.Should_Get_Encoder_Count++;
		}
	}
	else if((gpio_interrup & ENCODER_E2B_PIN)==ENCODER_E2B_PIN)
	{
		if(!DL_GPIO_readPins(ENCODER_PORT,ENCODER_E2A_PIN))
		{
			Encoder_B.Should_Get_Encoder_Count++;
		}
		else
		{
			Encoder_B.Should_Get_Encoder_Count--;
		}
	}
	DL_GPIO_clearInterruptStatus(ENCODER_PORT,KEY_PIN_1_PIN|ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);
}

//����������������
void TIMER_0_INST_IRQHandler(void)
{
	//�������ٶȼ���
	if( DL_TimerG_getPendingInterrupt(TIMER_0_INST) == DL_TIMER_IIDX_ZERO )
	{
        /* ���������װ�෴�����Ա�����ֵҲҪ�෴ */
        Encoder_A.Obtained_Get_Encoder_Count = Encoder_A.Should_Get_Encoder_Count;
        Encoder_B.Obtained_Get_Encoder_Count = -Encoder_B.Should_Get_Encoder_Count;
		
		//  �������ۼ����ܼ����������߼���
        total_left_pulses += Encoder_A.Obtained_Get_Encoder_Count;   // �ۼ�����������
        total_right_pulses += Encoder_B.Obtained_Get_Encoder_Count;  // �ۼ�����������

        /* ����������ֵ���� */
        Encoder_A.Should_Get_Encoder_Count = 0;
        Encoder_B.Should_Get_Encoder_Count = 0;

	}
}





