/**
 * @file test_helpers.h
 * @brief 測試輔助工具
 * @version 1.0.0
 * 
 * 提供常用的測試輔助函數,簡化測試程式碼
 * 注意: 函數名稱不使用 test_ 前綴,避免被 Ceedling 當作測試案例
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "unity.h"
#include "gaming_common.h"
#include "hal_interface.h"

// ========================================
// Mock HAL 輔助函數
// ========================================

/**
 * @brief 初始化測試用 Mock HAL
 * 
 * 設置 hal_ops 指標並重置所有 Mock 狀態
 */
void helpers_init_mock_hal(void);

/**
 * @brief 清理測試用 Mock HAL
 * 
 * 清除 hal_ops 指標並重置 Mock 狀態
 */
void helpers_cleanup_mock_hal(void);

/**
 * @brief 設置 Mock GPIO 的讀取值
 * 
 * @param pin GPIO 引腳編號
 * @param value 要返回的值 (HAL_GPIO_LOW 或 HAL_GPIO_HIGH)
 */
void helpers_set_gpio_value(int pin, hal_gpio_value_t value);

/**
 * @brief 設置 Mock ADC 的讀取值
 * 
 * @param value ADC 值 (0-1023)
 */
void helpers_set_adc_value(int value);

/**
 * @brief 驗證 GPIO 是否被正確初始化
 * 
 * @param pin GPIO 引腳編號
 * @param expected_direction 預期的方向 (HAL_GPIO_DIR_INPUT/OUTPUT)
 */
void helpers_verify_gpio_init(int pin, hal_gpio_dir_t expected_direction);

/**
 * @brief 驗證 GPIO 是否被寫入預期值
 * 
 * @param pin GPIO 引腳編號
 * @param expected_value 預期的值 (HAL_GPIO_LOW/HIGH)
 */
void helpers_verify_gpio_write(int pin, hal_gpio_value_t expected_value);

// ========================================
// LED 測試輔助函數
// ========================================

/**
 * @brief 驗證 LED 顏色是否正確設置
 * 
 * 檢查 RGB 三個 GPIO 的 PWM duty cycle 是否符合預期
 * 
 * @param expected_r 預期的紅色值 (0-255)
 * @param expected_g 預期的綠色值 (0-255)
 * @param expected_b 預期的藍色值 (0-255)
 */
void helpers_verify_led_color(uint8_t expected_r, 
                               uint8_t expected_g, 
                               uint8_t expected_b);

/**
 * @brief 驗證 LED 是否關閉
 * 
 * 檢查 RGB 三個 GPIO 的 PWM duty cycle 是否都為 0
 */
void helpers_verify_led_off(void);

// ========================================
// 斷言輔助巨集
// ========================================

/**
 * @brief 斷言函數返回 GAMING_OK
 */
#define TEST_ASSERT_GAMING_OK(func) \
    TEST_ASSERT_EQUAL(GAMING_OK, (func))

/**
 * @brief 斷言函數返回 GAMING_ERROR
 */
#define TEST_ASSERT_GAMING_ERROR(func) \
    TEST_ASSERT_EQUAL(GAMING_ERROR, (func))

/**
 * @brief 斷言函數返回指定錯誤碼
 */
#define TEST_ASSERT_GAMING_ERROR_CODE(expected_error, func) \
    TEST_ASSERT_EQUAL((expected_error), (func))

/**
 * @brief 斷言函數返回非負值(成功)
 */
#define TEST_ASSERT_SUCCESS(func) \
    TEST_ASSERT_GREATER_OR_EQUAL(0, (func))

/**
 * @brief 斷言函數返回負值(失敗)
 */
#define TEST_ASSERT_FAILURE(func) \
    TEST_ASSERT_LESS_THAN(0, (func))

// ========================================
// 顏色比較輔助函數
// ========================================

/**
 * @brief 比較兩個 LED 顏色是否相同
 * 
 * @param color1 第一個顏色
 * @param color2 第二個顏色
 * @return true 相同
 * @return false 不同
 */
bool helpers_colors_equal(const led_color_t *color1, 
                          const led_color_t *color2);

/**
 * @brief 驗證顏色是否符合預期
 * 
 * @param actual 實際顏色
 * @param expected_r 預期紅色值
 * @param expected_g 預期綠色值
 * @param expected_b 預期藍色值
 */
void helpers_assert_color(const led_color_t *actual,
                          uint8_t expected_r,
                          uint8_t expected_g,
                          uint8_t expected_b);

// ========================================
// 設備類型測試輔助
// ========================================

/**
 * @brief 設置 ADC 值以模擬 Client 裝置
 */
void helpers_simulate_client_device(void);

/**
 * @brief 設置 ADC 值以模擬 Server 裝置
 */
void helpers_simulate_server_device(void);

/**
 * @brief 驗證裝置類型
 * 
 * @param expected_type 預期的裝置類型
 */
void helpers_assert_device_type(device_type_t expected_type);

// ========================================
// 延時模擬
// ========================================

/**
 * @brief 模擬延時(不實際延時,僅用於測試)
 * 
 * @param ms 延時毫秒數
 */
void helpers_simulate_delay(int ms);

// ========================================
// 測試設置/拆卸輔助
// ========================================

/**
 * @brief 通用測試前置處理
 * 
 * 初始化 Mock HAL 和清理所有模組狀態
 */
void helpers_common_setup(void);

/**
 * @brief 通用測試後置處理
 * 
 * 清理所有模組和 Mock HAL
 */
void helpers_common_teardown(void);

#endif // TEST_HELPERS_H
