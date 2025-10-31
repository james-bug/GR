
#include "led_controller.h"
#include <stdio.h>
#include <stdbool.h>

// ========================================
// 內部狀態
// ========================================

static led_config_t current_config;
static bool is_initialized = false;

// 外部 hal_ops（由 hal_init.c 提供）
extern hal_ops_t *hal_ops;

// ========================================
// 內部輔助函數
// ========================================

// 將 0-255 的顏色值轉換為 GPIO 電平（簡化版：>127 為 HIGH）
static hal_gpio_value_t color_to_gpio_value(uint8_t color) {
    return (color > 127) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
}

// 設定單一 RGB 通道
static int set_rgb_channel(int pin, uint8_t value) {
    if (!hal_ops) {
        fprintf(stderr, "LED controller: HAL not initialized\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    hal_gpio_value_t gpio_val = color_to_gpio_value(value);
    int ret = hal_ops->gpio_write(pin, gpio_val);
    if (ret < 0) {
        fprintf(stderr, "LED controller: Failed to write GPIO%d\n", pin);
        return GAMING_ERROR_HAL_FAILED;
    }
    
    return GAMING_OK;
}

// ========================================
// LED 控制器初始化
// ========================================

int led_controller_init(const led_config_t *config) {
    if (config == NULL) {
        fprintf(stderr, "LED controller init: config is NULL\n");
        return GAMING_ERROR_INVALID_PARAM;
    }
    
    if (!hal_ops) {
        fprintf(stderr, "LED controller init: HAL not initialized\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    // 保存配置
    current_config = *config;
    
    // 初始化三個 GPIO 為輸出模式
    int ret;
    
    ret = hal_ops->gpio_init(config->pin_r, HAL_GPIO_DIR_OUTPUT);
    if (ret < 0) {
        fprintf(stderr, "LED controller: Failed to init R pin\n");
        return GAMING_ERROR_HAL_FAILED;
    }
    
    ret = hal_ops->gpio_init(config->pin_g, HAL_GPIO_DIR_OUTPUT);
    if (ret < 0) {
        fprintf(stderr, "LED controller: Failed to init G pin\n");
        return GAMING_ERROR_HAL_FAILED;
    }
    
    ret = hal_ops->gpio_init(config->pin_b, HAL_GPIO_DIR_OUTPUT);
    if (ret < 0) {
        fprintf(stderr, "LED controller: Failed to init B pin\n");
        return GAMING_ERROR_HAL_FAILED;
    }
    
    // 初始化為關閉狀態（全部 LOW）
    hal_ops->gpio_write(config->pin_r, HAL_GPIO_LOW);
    hal_ops->gpio_write(config->pin_g, HAL_GPIO_LOW);
    hal_ops->gpio_write(config->pin_b, HAL_GPIO_LOW);
    
    is_initialized = true;
    
    #ifdef DEBUG
    printf("LED controller initialized: R=%d, G=%d, B=%d\n",
           config->pin_r, config->pin_g, config->pin_b);
    #endif
    
    return GAMING_OK;
}

int led_controller_deinit(void) {
    if (!is_initialized) {
        return GAMING_OK;  // 已經清理過或未初始化
    }
    
    if (!hal_ops) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    // 關閉所有 LED
    led_off();
    
    // 清理 GPIO
    if (hal_ops->gpio_deinit) {
        hal_ops->gpio_deinit(current_config.pin_r);
        hal_ops->gpio_deinit(current_config.pin_g);
        hal_ops->gpio_deinit(current_config.pin_b);
    }
    
    is_initialized = false;
    
    return GAMING_OK;
}

// ========================================
// LED 基本控制
// ========================================

int led_set_color(uint8_t r, uint8_t g, uint8_t b) {
    if (!is_initialized) {
        fprintf(stderr, "LED controller: Not initialized\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    int ret;
    
    // 設定紅色通道
    ret = set_rgb_channel(current_config.pin_r, r);
    if (ret != GAMING_OK) {
        return ret;
    }
    
    // 設定綠色通道
    ret = set_rgb_channel(current_config.pin_g, g);
    if (ret != GAMING_OK) {
        return ret;
    }
    
    // 設定藍色通道
    ret = set_rgb_channel(current_config.pin_b, b);
    if (ret != GAMING_OK) {
        return ret;
    }
    
    #ifdef DEBUG
    printf("LED color set: R=%d, G=%d, B=%d\n", r, g, b);
    #endif
    
    return GAMING_OK;
}

int led_set_color_preset(led_color_t color) {
    return led_set_color(color.r, color.g, color.b);
}

int led_off(void) {
    return led_set_color(0, 0, 0);
}

// ========================================
// LED 狀態指示
// ========================================

int led_set_status(device_type_t device_type, ps5_state_t ps5_state) {
    if (!is_initialized) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    led_color_t color;
    
    // 根據設備類型和 PS5 狀態決定 LED 顏色
    if (device_type == DEVICE_TYPE_CLIENT) {
        // Client 模式（原 Travel Router）
        switch (ps5_state) {
            case PS5_STATE_ON:
                color = LED_COLOR_WHITE;    // PS5 開機：白色
                break;
            case PS5_STATE_STANDBY:
                color = LED_COLOR_ORANGE;   // PS5 待機：橙色
                break;
            case PS5_STATE_OFF:
            case PS5_STATE_UNKNOWN:
            default:
                color = LED_COLOR_BLACK;    // PS5 關機或未知：關閉
                break;
        }
    } else if (device_type == DEVICE_TYPE_SERVER) {
        // Server 模式（原 Home Router）
        switch (ps5_state) {
            case PS5_STATE_ON:
                color = LED_COLOR_GREEN;    // PS5 開機：綠色
                break;
            case PS5_STATE_STANDBY:
                color = LED_COLOR_BLUE;     // PS5 待機：藍色
                break;
            case PS5_STATE_OFF:
            case PS5_STATE_UNKNOWN:
            default:
                color = LED_COLOR_BLACK;    // PS5 關機或未知：關閉
                break;
        }
    } else {
        // 未知設備類型
        color = LED_COLOR_RED;  // 紅色表示錯誤
    }
    
    return led_set_color_preset(color);
}

int led_show_error(void) {
    // 簡單實作：顯示紅色
    // TODO: 實作閃爍效果（需要定時器或狀態機）
    return led_set_color_preset(LED_COLOR_RED);
}

int led_show_booting(void) {
    // 簡單實作：顯示白色
    // TODO: 實作呼吸燈效果（需要 PWM 或狀態機）
    return led_set_color_preset(LED_COLOR_WHITE);
}

// ========================================
// LED 特效（進階功能）
// ========================================

int led_blink(led_color_t color, int times, int interval_ms) {
    // TODO: 實作閃爍效果
    // 需要：
    // 1. 非阻塞式實作（使用狀態機 + 定時器）
    // 2. 或使用執行緒
    // 3. 或在主迴圈中輪詢
    
    // 暫時簡單實作：只設定顏色
    (void)times;      // 避免 unused warning
    (void)interval_ms;
    
    return led_set_color_preset(color);
}

int led_breathe(led_color_t color, int duration_ms) {
    // TODO: 實作呼吸燈效果
    // 需要：
    // 1. PWM 支援（調節亮度）
    // 2. 或快速切換模擬 PWM
    // 3. 狀態機管理漸變
    
    // 暫時簡單實作：只設定顏色
    (void)duration_ms;
    
    return led_set_color_preset(color);
}

int led_rainbow(int duration_ms) {
    // TODO: 實作彩虹效果
    // 需要：
    // 1. PWM 支援（混合顏色）
    // 2. 循環遍歷色環
    // 3. 狀態機管理過渡
    
    // 暫時未實作
    (void)duration_ms;
    
    fprintf(stderr, "LED rainbow effect not yet implemented\n");
    return GAMING_ERROR;
}



