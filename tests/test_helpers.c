/**
 * @file test_helpers.c
 * @brief 測試輔助函數實作
 * 
 * 提供常用的測試輔助函數,簡化測試程式碼
 * 
 * @author Gaming System Team
 * @date 2024-10-30
 * @version 1.1 - 修正版 (加入 hal_ops 定義)
 */

#include "test_helpers.h"
#include "mock_hal_interface.h"
#include <string.h>

// ========================================
// 測試用的 HAL (必須定義!)
// ========================================
hal_ops_t test_hal_ops_instance;
hal_ops_t *hal_ops = NULL;

// ========================================
// 內部狀態
// ========================================
static struct {
    int gpio_values[64];
    int adc_value;
} mock_state;

// ========================================
// Mock HAL 輔助函數
// ========================================

void helpers_init_mock_hal(hal_ops_t *hal_instance)
{
    if (hal_instance == NULL) {
        // 使用內建的測試實例
        test_hal_ops_instance.gpio_init = hal_gpio_init;
        test_hal_ops_instance.gpio_deinit = hal_gpio_deinit;
        test_hal_ops_instance.gpio_read = hal_gpio_read;
        test_hal_ops_instance.gpio_write = hal_gpio_write;
        test_hal_ops_instance.gpio_set_edge = hal_gpio_set_edge;
        test_hal_ops_instance.adc_read = hal_adc_read;
        
        hal_ops = &test_hal_ops_instance;
    } else {
        hal_ops = hal_instance;
    }
    
    // 清空狀態
    memset(&mock_state, 0, sizeof(mock_state));
}

void helpers_cleanup_mock_hal(void)
{
    hal_ops = NULL;
    memset(&mock_state, 0, sizeof(mock_state));
}

void helpers_set_gpio_value(int pin, hal_gpio_value_t value)
{
    if (pin >= 0 && pin < 64) {
        mock_state.gpio_values[pin] = value;
    }
}

int helpers_get_gpio_value(int pin)
{
    if (pin >= 0 && pin < 64) {
        return mock_state.gpio_values[pin];
    }
    return -1;
}

void helpers_set_adc_value(int value)
{
    mock_state.adc_value = value;
}

// ========================================
// 裝置類型測試輔助
// ========================================

void helpers_simulate_client_device(void)
{
    // Client 裝置 ADC 值 < 512
    helpers_set_adc_value(256);
}

void helpers_simulate_server_device(void)
{
    // Server 裝置 ADC 值 >= 512
    helpers_set_adc_value(768);
}

// ========================================
// LED 測試輔助
// ========================================

int helpers_verify_led_color(uint8_t expected_r, uint8_t expected_g, uint8_t expected_b)
{
    // 簡化版:檢查 GPIO 值是否符合預期
    // 實際實作需要根據 LED 控制器的邏輯來驗證
    
    // 這裡假設 LED_PIN_R = 17, LED_PIN_G = 18, LED_PIN_B = 19
    int r_gpio = helpers_get_gpio_value(17);
    int g_gpio = helpers_get_gpio_value(18);
    int b_gpio = helpers_get_gpio_value(19);
    
    // 簡化版:將 0-255 轉換為 GPIO 值 (>127 為 HIGH)
    int expected_r_gpio = (expected_r > 127) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    int expected_g_gpio = (expected_g > 127) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    int expected_b_gpio = (expected_b > 127) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    
    return (r_gpio == expected_r_gpio) && 
           (g_gpio == expected_g_gpio) && 
           (b_gpio == expected_b_gpio);
}
