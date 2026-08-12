#include "key_handler.h"

//#include "systick_config.h"  // 包含systick的全局变量g_systick_count
extern volatile uint32_t g_systick_count;
// 读取按键引脚状态（低电平有效，根据硬件调整）
static uint8_t Key_ReadPin(Key* key) {
    // 读取引脚状态：按下返回1，释放返回0（消抖前的原始状态）
    return (DL_GPIO_readPins(key->port, key->pin) == 0) ? 1 : 0;
}

// 初始化按键
void Key_Init(Key* keys, uint32_t debounce_time, uint32_t long_press_time) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        keys[i].current_state = KEY_STATE_RELEASED;
        keys[i].last_state = KEY_STATE_RELEASED;
        keys[i].pressed_flag = false;
        keys[i].released_flag = false;
        keys[i].short_press = false;
        keys[i].long_press = false;
        keys[i].long_press_triggered = false;
        keys[i].press_time = 0;
        keys[i].debounce_time = debounce_time;    // 消抖时间（如20ms）
        keys[i].long_press_time = long_press_time;// 长按时间（如1000ms）
    }
}

// 按键扫描（非阻塞，需定期调用，建议每10ms一次）
void Key_Scan(Key* keys) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        uint8_t raw_state = Key_ReadPin(&keys[i]);  // 读取原始状态
        uint32_t current_time = g_systick_count;    // 获取当前时间戳

        // 消抖逻辑：状态稳定超过debounce_time才更新状态
        if (raw_state != keys[i].last_state) {
            keys[i].press_time = current_time;  // 状态变化时重置计时
        } else if (current_time - keys[i].press_time >= keys[i].debounce_time) {
            // 消抖完成，更新状态
            if (raw_state == 1) {  // 按键按下（消抖后）
                if (keys[i].current_state != KEY_STATE_PRESSED && 
                    keys[i].current_state != KEY_STATE_LONG_PRESS) {
                    keys[i].current_state = KEY_STATE_PRESSED;
                    keys[i].pressed_flag = true;
                    keys[i].press_time = current_time;  // 记录按下时间
                }
            } else {  // 按键释放（消抖后）
                if (keys[i].current_state != KEY_STATE_RELEASED) {
                    keys[i].current_state = KEY_STATE_RELEASED;
                    keys[i].released_flag = true;
                    // 释放时若未触发长按，则标记为短按
                    if (!keys[i].long_press_triggered) {
                        keys[i].short_press = true;
                    }
                    keys[i].long_press = false;
                    keys[i].long_press_triggered = false;
                }
            }
        }

        // 长按判断（仅在按下状态且未触发过长按时检查）
        if (keys[i].current_state == KEY_STATE_PRESSED && !keys[i].long_press_triggered) {
            if (current_time - keys[i].press_time >= keys[i].long_press_time) {
                keys[i].current_state = KEY_STATE_LONG_PRESS;
                keys[i].long_press = true;
                keys[i].long_press_triggered = true;  // 标记为已触发
            }
        }

        keys[i].last_state = raw_state;  // 更新上一次原始状态
    }
}

// 获取短按状态（一次性，调用后清除标志）
bool Key_GetShortPress(Key* key) {
    if (key->short_press) {
        key->short_press = false;
        return true;
    }
    return false;
}

// 获取长按状态（持续有效，直到释放）
bool Key_GetLongPress(Key* key) {
    return key->long_press;
}

// 检查按键是否按下（消抖后）
bool Key_IsPressed(Key* key) {
    if (key->pressed_flag) {
        key->pressed_flag = false;
        return true;
    }
    return false;
}

// 检查按键是否释放（消抖后）
bool Key_IsReleased(Key* key) {
    if (key->released_flag) {
        key->released_flag = false;
        return true;
    }
    return false;
}
