#ifndef HAL_INTERFACE_H
#define HAL_INTERFACE_H

#include <stdint.h>

// ========================================
// GPIO 方向定義
// ========================================
typedef enum {
    HAL_GPIO_DIR_INPUT = 0,
    HAL_GPIO_DIR_OUTPUT = 1
} hal_gpio_dir_t;

// ========================================
// GPIO 值定義
// ========================================
typedef enum {
    HAL_GPIO_LOW = 0,
    HAL_GPIO_HIGH = 1
} hal_gpio_value_t;

// ========================================
// HAL 函數原型（供 CMock 使用）
// ========================================

// GPIO 操作
int hal_gpio_init(int pin, hal_gpio_dir_t direction);
int hal_gpio_deinit(int pin);
int hal_gpio_read(int pin);
int hal_gpio_write(int pin, hal_gpio_value_t value);
int hal_gpio_set_edge(int pin, const char *edge);

// ADC 操作
int hal_adc_read(const char *device);

// PWM 操作 (for LED)
int hal_pwm_init(int pin, int frequency);
int hal_pwm_set_duty(int pin, int duty_percent);
int hal_pwm_deinit(int pin);

// 系統資訊
const char* hal_get_impl_name(void);

// ========================================
// HAL 操作函數指標結構（用於實際運行）
// ========================================
typedef struct {
    int (*gpio_init)(int pin, hal_gpio_dir_t direction);
    int (*gpio_deinit)(int pin);
    int (*gpio_read)(int pin);
    int (*gpio_write)(int pin, hal_gpio_value_t value);
    int (*gpio_set_edge)(int pin, const char *edge);
    int (*adc_read)(const char *device);
    int (*pwm_init)(int pin, int frequency);
    int (*pwm_set_duty)(int pin, int duty_percent);
    int (*pwm_deinit)(int pin);
    const char* (*get_impl_name)(void);
} hal_ops_t;

// ========================================
// 全域 HAL 操作表指標
// ========================================
extern hal_ops_t *hal_ops;

// ========================================
// HAL 初始化函數
// ========================================
int hal_init(const char *mode);
void hal_cleanup(void);

// ========================================
// Mock HAL 輔助函數（僅用於測試）
// ========================================
#ifdef TEST
void mock_hal_set_adc_value(int value);
void mock_hal_set_gpio_value(int pin, hal_gpio_value_t value);
int mock_hal_get_gpio_value(int pin);
void mock_hal_reset(void);
#endif

#endif // HAL_INTERFACE_H
