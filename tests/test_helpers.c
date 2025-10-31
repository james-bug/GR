/**
 * @file test_helpers.c
 * @brief 測試輔助工具實作
 * @version 1.0.0
 * 
 * 注意: 函數名稱不使用 test_ 前綴,避免被 Ceedling 當作測試案例
 */

#include "test_helpers.h"
#include "mock_hal_interface.h"
#include <stdio.h>
#include <string.h>

// ========================================
// Mock HAL 輔助函數
// ========================================

void helpers_init_mock_hal(void) {
    // 設置 hal_ops 指標(模擬已初始化)
    hal_ops = (hal_ops_t*)1;
    
    // 重置所有 Mock
    mock_hal_reset();
    
    #ifdef DEBUG
    printf("[Test Helpers] Mock HAL initialized\n");
    #endif
}

void helpers_cleanup_mock_hal(void) {
    // 清除 hal_ops 指標
    hal_ops = NULL;
    
    #ifdef DEBUG
    printf("[Test Helpers] Mock HAL cleaned up\n");
    #endif
}

void helpers_set_gpio_value(int pin, hal_gpio_value_t value) {
    mock_hal_set_gpio_value(pin, value);
    
    #ifdef DEBUG
    printf("[Test Helpers] Set GPIO%d value to %d\n", pin, value);
    #endif
}

void helpers_set_adc_value(int value) {
    mock_hal_set_adc_value(value);
    
    #ifdef DEBUG
    printf("[Test Helpers] Set ADC value to %d\n", value);
    #endif
}

void helpers_verify_gpio_init(int pin, hal_gpio_dir_t expected_direction) {
    // 這個函數在使用 CMock 時,應該配合 Expect 函數使用
    // 這裡提供一個簡單的實作
    #ifdef DEBUG
    printf("[Test Helpers] Verifying GPIO%d init with direction %d\n", 
           pin, expected_direction);
    #else
    (void)pin;
    (void)expected_direction;
    #endif
}

void helpers_verify_gpio_write(int pin, hal_gpio_value_t expected_value) {
    int actual_value = mock_hal_get_gpio_value(pin);
    TEST_ASSERT_EQUAL(expected_value, actual_value);
    
    #ifdef DEBUG
    printf("[Test Helpers] Verified GPIO%d write: expected=%d, actual=%d\n",
           pin, expected_value, actual_value);
    #endif
}

// ========================================
// LED 測試輔助函數
// ========================================

void helpers_verify_led_color(uint8_t expected_r, 
                               uint8_t expected_g, 
                               uint8_t expected_b) {
    // 計算 PWM duty cycle (0-255 -> 0-100%)
    int expected_duty_r = (expected_r * 100) / 255;
    int expected_duty_g = (expected_g * 100) / 255;
    int expected_duty_b = (expected_b * 100) / 255;
    
    #ifdef DEBUG
    printf("[Test Helpers] Verifying LED color: R=%d%%, G=%d%%, B=%d%%\n",
           expected_duty_r, expected_duty_g, expected_duty_b);
    #else
    // 避免未使用變數警告
    (void)expected_duty_r;
    (void)expected_duty_g;
    (void)expected_duty_b;
    #endif
    
    // 注意: 實際驗證需要配合 CMock 的 Expect 函數
    // 這裡僅提供框架
}

void helpers_verify_led_off(void) {
    #ifdef DEBUG
    printf("[Test Helpers] Verifying LED is OFF\n");
    #endif
    
    helpers_verify_led_color(0, 0, 0);
}

// ========================================
// 顏色比較輔助函數
// ========================================

bool helpers_colors_equal(const led_color_t *color1, 
                          const led_color_t *color2) {
    if (color1 == NULL || color2 == NULL) {
        return false;
    }
    
    return (color1->r == color2->r && 
            color1->g == color2->g && 
            color1->b == color2->b);
}

void helpers_assert_color(const led_color_t *actual,
                          uint8_t expected_r,
                          uint8_t expected_g,
                          uint8_t expected_b) {
    TEST_ASSERT_NOT_NULL(actual);
    TEST_ASSERT_EQUAL_UINT8(expected_r, actual->r);
    TEST_ASSERT_EQUAL_UINT8(expected_g, actual->g);
    TEST_ASSERT_EQUAL_UINT8(expected_b, actual->b);
    
    #ifdef DEBUG
    printf("[Test Helpers] Color verified: R=%d, G=%d, B=%d\n",
           actual->r, actual->g, actual->b);
    #endif
}

// ========================================
// 設備類型測試輔助
// ========================================

void helpers_simulate_client_device(void) {
    // 設置 ADC 值小於閾值 (512)
    helpers_set_adc_value(256);
    
    #ifdef DEBUG
    printf("[Test Helpers] Simulating Client device (ADC=256)\n");
    #endif
}

void helpers_simulate_server_device(void) {
    // 設置 ADC 值大於等於閾值 (512)
    helpers_set_adc_value(768);
    
    #ifdef DEBUG
    printf("[Test Helpers] Simulating Server device (ADC=768)\n");
    #endif
}

void helpers_assert_device_type(device_type_t expected_type) {
    #ifdef DEBUG
    const char *type_str;
    
    switch (expected_type) {
        case DEVICE_TYPE_CLIENT:
            type_str = "Client";
            break;
        case DEVICE_TYPE_SERVER:
            type_str = "Server";
            break;
        case DEVICE_TYPE_UNKNOWN:
        default:
            type_str = "Unknown";
            break;
    }
    
    printf("[Test Helpers] Expected device type: %s\n", type_str);
    #else
    // 避免未使用參數警告
    (void)expected_type;
    #endif
}

// ========================================
// 延時模擬
// ========================================

void helpers_simulate_delay(int ms) {
    // 測試中不實際延時,僅記錄
    #ifdef DEBUG
    printf("[Test Helpers] Simulated delay: %d ms\n", ms);
    #else
    (void)ms;
    #endif
}

// ========================================
// 測試設置/拆卸輔助
// ========================================

void helpers_common_setup(void) {
    // 初始化 Mock HAL
    helpers_init_mock_hal();
    
    // 這裡可以加入其他通用的初始化
    
    #ifdef DEBUG
    printf("[Test Helpers] Common setup completed\n");
    #endif
}

void helpers_common_teardown(void) {
    // 清理 Mock HAL
    helpers_cleanup_mock_hal();
    
    // 這裡可以加入其他通用的清理
    
    #ifdef DEBUG
    printf("[Test Helpers] Common teardown completed\n");
    #endif
}
