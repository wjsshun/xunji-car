#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H

#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include <stdint.h>

// ��������������ʵ�������޸ģ�
#define KEY_COUNT 3

// ����״̬ö�٣�����ȱʧ��״̬���壩
typedef enum {
    KEY_STATE_RELEASED = 0,  // �����ͷ�
    KEY_STATE_PRESSED,       // �������£�������
    KEY_STATE_LONG_PRESS     // �����������Ѵ���������
} KeyState;

// �����ṹ�壨ͳһ��Ա���ƣ���ʵ�ִ���ƥ�䣩
typedef struct {
    GPIO_Regs* port;         // GPIO�˿ڣ���GPIOB��
    uint32_t pin;            // GPIO���ţ���GPIO_PIN_14��
    KeyState current_state;  // ��ǰ״̬
    KeyState last_state;     // ��һ��״̬
    bool pressed_flag;       // ���±�־��һ���ԣ�
    bool released_flag;      // �ͷű�־��һ���ԣ�
    bool short_press;        // �̰���־��һ���ԣ�
    bool long_press;         // ������־��������Ч��
    bool long_press_triggered; // �����Ƿ��Ѵ���
    uint32_t press_time;     // ����ʱ���������systick��
    uint32_t debounce_time;  // ����ʱ�䣨ms��
    uint32_t long_press_time;// �����ж�ʱ�䣨ms��
} Key;

// ��������
void Key_Init(Key* keys, uint32_t debounce_time, uint32_t long_press_time);
void Key_Scan(Key* keys);
bool Key_GetShortPress(Key* key);
bool Key_GetLongPress(Key* key);
bool Key_IsPressed(Key* key);
bool Key_IsReleased(Key* key);

#endif // KEY_HANDLER_H