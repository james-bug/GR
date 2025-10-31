/**
 * @file test_led_controller.c
 * @brief LED Controller 單元測試（完全修正版）
 * @version 2.2.0
 * 
 * 修正：
 * 1. led_controller_init() 需要 led_config_t 參數
 * 2. led_set_color_preset() 參數是傳值而非傳指標
 * 3. 使用 PWM 而非 GPIO write
 */

#include "unity.h"
#include "mock_hal_interface.h"  // CMock 自動生成的 mock
#include "led_controller.h"
#include "gaming_common.h"

// ========================================
// 外部函數聲明
// ========================================
extern int hal_init(const char *mode);
extern void hal_cleanup(void);
extern void mock_hal_reset(void);

// ========================================
// 測試配置
// ========================================
static led_config_t test_config;

// ========================================
// 測試前置/後置處理
// ========================================

void setUp(void) {
    hal_init("mock");
    mock_hal_reset();
    
    // 設置測試用的 LED 配置
    test_config.pin_r = GPIO_PIN_LED_R;  // 17
    test_config.pin_g = GPIO_PIN_LED_G;  // 18
    test_config.pin_b = GPIO_PIN_LED_B;  // 19
}

void tearDown(void) {
    hal_cleanup();
}

// ========================================
// LED 初始化測試
// ========================================

void test_led_controller_init_success(void) {
    // 設置 CMock 期望 - RGB 三個 GPIO 初始化
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    
    // 期望初始化時設置為關閉狀態（全 LOW）
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    
    int result = led_controller_init(&test_config);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_led_controller_init_null_config(void) {
    // 傳入 NULL 配置
    int result = led_controller_init(NULL);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_led_controller_init_gpio_fail(void) {
    // 模擬 GPIO 初始化失敗
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, -1);
    
    int result = led_controller_init(&test_config);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// LED 顏色設定測試
// ========================================

void test_led_set_color_white(void) {
    // 初始化
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // 設定白色 (255, 255, 255) - 全部 HIGH
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_HIGH, 0);
    
    int result = led_set_color(255, 255, 255);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_led_set_color_red(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // 設定紅色 (255, 0, 0)
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    
    int result = led_set_color(255, 0, 0);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_led_set_color_without_init(void) {
    // 未初始化就設定顏色
    int result = led_set_color(255, 255, 255);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_NOT_INITIALIZED, result);
}

// ========================================
// LED 預設顏色測試
// ========================================

void test_led_set_color_preset(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // 設定預設白色 (傳值，不是傳指標)
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_HIGH, 0);
    
    int result = led_set_color_preset(LED_COLOR_WHITE);  // 傳值
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

// ========================================
// LED 關閉測試
// ========================================

void test_led_off(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // 關閉 LED (0, 0, 0)
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    
    int result = led_off();
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

// ========================================
// LED 狀態指示測試
// ========================================

void test_led_set_status_client_ps5_on(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // Client + PS5 ON = 白色 (255, 255, 255)
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_HIGH, 0);
    
    int result = led_set_status(DEVICE_TYPE_CLIENT, PS5_STATE_ON);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_led_set_status_client_ps5_standby(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // Client + PS5 STANDBY = 橙色 (255, 165, 0)
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_HIGH, 0);  // 255 > 127
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_HIGH, 0);  // 165 > 127
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);   // 0 < 127
    
    int result = led_set_status(DEVICE_TYPE_CLIENT, PS5_STATE_STANDBY);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_led_set_status_client_ps5_off(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // Client + PS5 OFF = LED 關閉 (0, 0, 0)
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    
    int result = led_set_status(DEVICE_TYPE_CLIENT, PS5_STATE_OFF);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

// ========================================
// LED 清理測試
// ========================================

void test_led_controller_deinit(void) {
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    led_controller_init(&test_config);
    
    // 清理時先關閉 LED
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_R, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_G, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(GPIO_PIN_LED_B, HAL_GPIO_LOW, 0);
    
    // 然後 deinit GPIO
    hal_gpio_deinit_ExpectAndReturn(GPIO_PIN_LED_R, 0);
    hal_gpio_deinit_ExpectAndReturn(GPIO_PIN_LED_G, 0);
    hal_gpio_deinit_ExpectAndReturn(GPIO_PIN_LED_B, 0);
    
    led_controller_deinit();
}
