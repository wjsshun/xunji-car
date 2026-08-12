// oled_menu.h
#ifndef __OLED_MENU_H
#define __OLED_MENU_H

#include "stdint.h"
#include <stdbool.h>

// 编码器距离显示值 (extern for track module)
extern float encod;

// 运行时间 (秒)
extern volatile uint32_t run_time_seconds;

// 模式状态行刷新 (状态切换时调用, 全刷行1-2)
void OLED_UpdateModeLines(void);

// 完整菜单刷新 + 初始化
void OLED_UpdateMenu(void);
void OLED_MenuInit(void);

#endif
