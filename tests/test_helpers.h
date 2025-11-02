/**
 * @file test_helpers.h
 * @brief 測試輔助函數標頭檔
 * 
 * 提供常用的測試輔助函數,簡化測試程式碼
 * 
 * @author Gaming System Team
 * @date 2024-10-30
 * @version 1.0
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "hal_interface.h"
#include "gaming_common.h"

// ========================================
// Mock HAL 輔助函數
// ========================================

/**
 * @brief 初始化 Mock HAL
 * 
 * 設置所有 HAL 函數指標到 mock 函數
 * 
 * @param hal_instance HAL 實例指標
 */
void helpers_init_mock_hal(hal_ops_t *hal_instance);

/**
 * @brief 清理 Mock HAL
 * 
 * 清理 HAL 狀態
 */
void helpers_cleanup_mock_hal(void);

/**
 * @brief 設置 GPIO 模擬值
 * 
 * @param pin GPIO 引腳號
 * @param value GPIO 值
 */
void helpers_set_gpio_value(int pin, hal_gpio_value_t value);

/**
 * @brief 取得 GPIO 模擬值
 * 
 * @param pin GPIO 引腳號
 * @return GPIO 值
 */
int helpers_get_gpio_value(int pin);

/**
 * @brief 設置 ADC 模擬值
 * 
 * @param value ADC 值
 */
void helpers_set_adc_value(int value);

// ========================================
// 裝置類型測試輔助
// ========================================

/**
 * @brief 模擬 Client 裝置
 * 
 * 設置 ADC 值為 Client 範圍 (< 512)
 */
void helpers_simulate_client_device(void);

/**
 * @brief 模擬 Server 裝置
 * 
 * 設置 ADC 值為 Server 範圍 (>= 512)
 */
void helpers_simulate_server_device(void);

// ========================================
// LED 測試輔助
// ========================================

/**
 * @brief 驗證 LED 顏色
 * 
 * @param expected_r 預期紅色值 (0-255)
 * @param expected_g 預期綠色值 (0-255)
 * @param expected_b 預期藍色值 (0-255)
 * @return 1 if匹配, 0 otherwise
 */
int helpers_verify_led_color(uint8_t expected_r, uint8_t expected_g, uint8_t expected_b);

#endif // TEST_HELPERS_H
