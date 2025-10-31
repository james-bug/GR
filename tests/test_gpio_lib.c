/**
 * @file test_gpio_lib.c
 * @brief GPIO Library 單元測試（最終修正版）
 * @version 2.1.0
 */

#include "unity.h"
#include "mock_hal_interface.h"  // CMock 自動生成的 mock
#include "gpio_lib.h"
#include "gaming_common.h"

// ========================================
// 外部函數聲明（來自 hal_init.c 和 hal_mock.c）
// 重要：類型必須與 hal_interface.h 中的聲明完全一致
// ========================================
extern int hal_init(const char *mode);
extern void hal_cleanup(void);
extern void mock_hal_reset(void);
extern int mock_hal_get_gpio_value(int pin);  // ← 返回 int，不是 hal_gpio_value_t
extern bool mock_hal_is_gpio_initialized(int pin);
extern hal_gpio_dir_t mock_hal_get_gpio_direction(int pin);

// ========================================
// 測試前置/後置處理
// ========================================

void setUp(void) {
    // 使用真實的 hal_init 和 mock_hal_reset（來自 hal_mock.c）
    hal_init("mock");
    mock_hal_reset();
}

void tearDown(void) {
    hal_cleanup();
}

// ========================================
// GPIO 輸出初始化測試
// ========================================

void test_gpio_init_output_success(void) {
    // 設置 CMock 期望
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    
    // 執行測試
    int result = gpio_lib_init_output(GPIO_PIN_LED_R);
    
    // 驗證
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_gpio_init_output_invalid_pin(void) {
    int result = gpio_lib_init_output(-1);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_gpio_init_output_fail(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, -1);
    
    int result = gpio_lib_init_output(GPIO_PIN_LED_R);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 輸入初始化測試
// ========================================

void test_gpio_init_input_success(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_DIR_INPUT, 0);
    
    int result = gpio_lib_init_input(GPIO_PIN_BUTTON);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_gpio_init_input_fail(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_DIR_INPUT, -1);
    
    int result = gpio_lib_init_input(GPIO_PIN_BUTTON);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 讀取測試
// ========================================

void test_gpio_read_high(void) {
    // 設置
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_DIR_INPUT, 0);
    gpio_lib_init_input(GPIO_PIN_BUTTON);
    
    // 模擬讀取
    hal_gpio_read_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_HIGH);
    
    // 執行
    int value = gpio_lib_read(GPIO_PIN_BUTTON);
    
    // 驗證
    TEST_ASSERT_EQUAL(1, value);
}

void test_gpio_read_low(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_DIR_INPUT, 0);
    gpio_lib_init_input(GPIO_PIN_BUTTON);
    
    hal_gpio_read_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_LOW);
    
    int value = gpio_lib_read(GPIO_PIN_BUTTON);
    
    TEST_ASSERT_EQUAL(0, value);
}

void test_gpio_read_fail(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_BUTTON, HAL_GPIO_DIR_INPUT, 0);
    gpio_lib_init_input(GPIO_PIN_BUTTON);
    
    hal_gpio_read_ExpectAndReturn(GPIO_PIN_BUTTON, -1);
    
    int value = gpio_lib_read(GPIO_PIN_BUTTON);
    
    TEST_ASSERT_LESS_THAN(0, value);
}

// ========================================
// GPIO 寫入測試
// ========================================

void test_gpio_write_high(void) {
    // 初始化
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    // 寫入
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);
    
    int result = gpio_lib_write(GPIO_PIN_LED_R, 1);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_gpio_write_low(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    
    int result = gpio_lib_write(GPIO_PIN_LED_R, 0);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_gpio_write_fail(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, -1);
    
    int result = gpio_lib_write(GPIO_PIN_LED_R, 1);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 反轉測試
// ========================================

void test_gpio_toggle_low_to_high(void) {
    // 初始化
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    // 讀取當前值（假設為 LOW）
    hal_gpio_read_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW);
    
    // 寫入 HIGH
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);
    
    int result = gpio_lib_toggle(GPIO_PIN_LED_R);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_gpio_toggle_high_to_low(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    // 讀取當前值（假設為 HIGH）
    hal_gpio_read_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH);
    
    // 寫入 LOW
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    
    int result = gpio_lib_toggle(GPIO_PIN_LED_R);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

// ========================================
// 錯誤處理測試
// ========================================

void test_gpio_read_without_init(void) {
    // 未初始化就讀取
    int value = gpio_lib_read(GPIO_PIN_BUTTON);
    
    TEST_ASSERT_LESS_THAN(0, value);
}

void test_gpio_write_without_init(void) {
    // 未初始化就寫入
    int result = gpio_lib_write(GPIO_PIN_LED_R, 1);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_NOT_INITIALIZED, result);
}

// ========================================
// GPIO 清理測試
// ========================================

void test_gpio_cleanup_success(void) {
    // 初始化
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    // 清理
    hal_gpio_deinit_ExpectAndReturn(GPIO_PIN_LED_R, 0);
    
    gpio_lib_cleanup(GPIO_PIN_LED_R);
    
    // CMock 會自動驗證期望被調用
}

void test_gpio_cleanup_fail(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    gpio_lib_init_output(GPIO_PIN_LED_R);
    
    hal_gpio_deinit_ExpectAndReturn(GPIO_PIN_LED_R, -1);
    
    gpio_lib_cleanup(GPIO_PIN_LED_R);
}
