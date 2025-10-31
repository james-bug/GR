#ifndef GPIO_LIB_H
#define GPIO_LIB_H

#include "gaming_common.h"
#include "hal_interface.h"

// ========================================
// GPIO 庫初始化與配置
// ========================================

// 初始化 GPIO 為輸出模式
int gpio_lib_init_output(int pin);

// 初始化 GPIO 為輸入模式
int gpio_lib_init_input(int pin);

// 初始化 GPIO 為輸入模式（帶中斷）
// edge: "none", "rising", "falling", "both"
int gpio_lib_init_input_irq(int pin, const char *edge);

// ========================================
// GPIO 操作函數
// ========================================

// 讀取 GPIO 值（返回 0 或 1，錯誤返回負值）
int gpio_lib_read(int pin);

// 寫入 GPIO 值（value: 0 或 1）
int gpio_lib_write(int pin, int value);

// 反轉 GPIO 值
int gpio_lib_toggle(int pin);

// ========================================
// GPIO 清理
// ========================================

// 清理 GPIO（unexport）
int gpio_lib_cleanup(int pin);

#endif // GPIO_LIB_H
