#include "bsp_oled.h"
#include <string.h>
#include "oled_menu.h"
#include "bsp_track.h"
#include "bsp_motor_hallencoder.h"
#include "board.h"

float encod;

/*
 * OLED 8行布局 (全6x8字体, 每行8px, 64px满屏):
 *   y=0:  "=== 2026H-v1 ==="  (标题)
 *   y=8:  "Dist: OFF/ON "     (固定距离模式状态)
 *   y=16: "Lap : OFF/ON "     (跑一圈模式状态)
 *   y=24: "TR:XXXXXXXX+NN"    (8路传感器+误差)
 *   y=32: "E:XXXXX D:X.XXm"   (编码器脉冲+距离m)
 *   y=40: "B3: OFF/ON "       (按键3状态)
 *   y=48: "---"               (预留)
 *   y=56: "T: XXXXXs"         (运行时间, 秒)
 */

// 更新模式状态行 (行1 Dist, 行2 Lap)
void OLED_UpdateModeLines(void)
{
    // 行1 (y=8): "Dist:" + 状态
    OLED_ShowString(0, 8, (u8*)"Dist:   ", 8, 1);
    OLED_ShowString(36, 8, (u8*)(current_mode == MODE_DISTANCE ? "ON " : "OFF"), 8, 1);

    // 行2 (y=16): "Lap :" + 状态
    OLED_ShowString(0, 16, (u8*)"Lap :   ", 8, 1);
    OLED_ShowString(36, 16, (u8*)(current_mode == MODE_ONELAP ? "ON " : "OFF"), 8, 1);

    // 行5 (y=40): "B3:" + 状态
    OLED_ShowString(0, 40, (u8*)"B3:", 8, 1);
    OLED_ShowString(24, 40, (u8*)(button3_state ? "ON " : "OFF"), 8, 1);

    // 刷新受影响的行
    OLED_RefreshRegion(1, 0, 60);
    OLED_RefreshRegion(2, 0, 60);
    OLED_RefreshRegion(5, 0, 42);
}

// 完整更新菜单 (首次初始化时调用)
void OLED_UpdateMenu(void)
{
    OLED_Clear();

    // 行0 (y=0): 标题
    OLED_ShowString(16, 0, (u8*)"=== 2026H-v1 ===", 8, 1);

    // 行1 (y=8): "Dist: OFF"
    OLED_ShowString(0, 8, (u8*)"Dist:", 8, 1);
    OLED_ShowString(36, 8, (u8*)"OFF", 8, 1);

    // 行2 (y=16): "Lap : OFF"
    OLED_ShowString(0, 16, (u8*)"Lap :", 8, 1);
    OLED_ShowString(36, 16, (u8*)"OFF", 8, 1);

    // 行3 (y=24): "TR:" 占位
    OLED_ShowString(0, 24, (u8*)"TR:", 8, 1);

    // 行4 (y=32): "E:" 占位
    OLED_ShowString(0, 32, (u8*)"E:     D:", 8, 1);

    // 行5 (y=40): "B3: OFF"
    OLED_ShowString(0, 40, (u8*)"B3:", 8, 1);
    OLED_ShowString(24, 40, (u8*)"OFF", 8, 1);

    // 行6 (y=48): 预留
    OLED_ShowString(0, 48, (u8*)"---", 8, 1);

    // 行7 (y=56): "T:" 占位
    OLED_ShowString(0, 56, (u8*)"T:", 8, 1);

    OLED_Refresh();
}

// 初始化菜单
void OLED_MenuInit(void)
{
    OLED_Init();
    OLED_DisPlay_On();
    OLED_UpdateMenu();
}
