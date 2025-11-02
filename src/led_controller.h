

#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "gaming_common.h"
#include "hal_interface.h"

// ========================================
// LED 配置
// ========================================

typedef struct {
    int pin_r;  // 紅色 LED GPIO pin
    int pin_g;  // 綠色 LED GPIO pin
    int pin_b;  // 藍色 LED GPIO pin
} led_config_t;

// ========================================
// LED 控制器初始化
// ========================================

// 初始化 LED 控制器
int led_controller_init(const led_config_t *config);

// 清理 LED 控制器
int led_controller_deinit(void);

// ========================================
// LED 基本控制
// ========================================

// 設定 LED 顏色（RGB 0-255）
int led_set_color(uint8_t r, uint8_t g, uint8_t b);

// 使用預定義顏色
int led_set_color_preset(led_color_t color);

// 關閉 LED（全黑）
int led_off(void);

// ========================================
// LED 狀態指示
// ========================================

// 根據系統狀態設定 LED
int led_set_status(device_type_t device_type, ps5_state_t ps5_state);

// 顯示錯誤狀態（紅色閃爍）
int led_show_error(void);

// 顯示啟動中（白色呼吸燈）
int led_show_booting(void);

// ========================================
// LED 特效（可選）
// ========================================

// 閃爍效果
int led_blink(led_color_t color, int times, int interval_ms);

// 呼吸燈效果
int led_breathe(led_color_t color, int duration_ms);

// 彩虹效果
int led_rainbow(int duration_ms);

#endif // LED_CONTROLLER_H



