/**
 * @file test_led_controller.c
 * @brief LED Controller 單元測試
 * 
 * 測試 LED 初始化、顏色設定、狀態指示等功能
 * 
 * @author Gaming System Team
 * @date 2024-10-30
 * @version 1.1 - 修正版 (確保 hal_ops 定義正確)
 */

#include "unity.h"
#include "mock_hal_interface.h"
#include "led_controller.h"
#include "gaming_common.h"

// ========================================
// 測試用的 HAL (必須定義!)
// ========================================
hal_ops_t test_hal_ops_instance;
hal_ops_t *hal_ops = NULL;

// ========================================
// 測試配置
// ========================================
static led_config_t test_led_config = {
    .pin_r = 17,
    .pin_g = 18,
    .pin_b = 19
};

// ========================================
// 測試設置與拆卸
// ========================================
void setUp(void)
{
    // 設置 mock HAL
    test_hal_ops_instance.gpio_init = hal_gpio_init;
    test_hal_ops_instance.gpio_write = hal_gpio_write;
    test_hal_ops_instance.gpio_deinit = hal_gpio_deinit;
    hal_ops = &test_hal_ops_instance;
}

void tearDown(void)
{
    // 清理
}

// ========================================
// LED 初始化測試
// ========================================

void test_led_controller_init_should_initialize_all_pins(void)
{
    // 預期初始化三個 GPIO 為輸出模式
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    
    // 預期全部設為 LOW（關閉 LED）
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    
    int result = led_controller_init(&test_led_config);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_led_controller_init_should_fail_with_null_config(void)
{
    int result = led_controller_init(NULL);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_INVALID_PARAM, result);
}

void test_led_controller_init_should_fail_when_hal_not_initialized(void)
{
    // 設置 hal_ops 為 NULL
    hal_ops = NULL;
    
    int result = led_controller_init(&test_led_config);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_led_controller_init_should_fail_when_gpio_init_fails(void)
{
    // 第一個 GPIO 初始化失敗
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, -1);
    
    int result = led_controller_init(&test_led_config);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// LED 顏色設定測試
// ========================================

void test_led_controller_set_color_white(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 設定白色 (255, 255, 255) -> 全部 HIGH
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_HIGH, 0);
    
    int result = led_controller_set_color(255, 255, 255);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_led_controller_set_color_red(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 設定紅色 (255, 0, 0) -> R=HIGH, G=LOW, B=LOW
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    
    int result = led_controller_set_color(255, 0, 0);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_led_controller_set_color_off(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 設定黑色 (0, 0, 0) -> 全部 LOW
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    
    int result = led_controller_set_color(0, 0, 0);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

// ========================================
// LED 預設顏色測試
// ========================================

void test_led_controller_set_color_preset_white(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 設定白色預設
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_HIGH, 0);
    
    led_color_t white = LED_COLOR_WHITE;
    int result = led_controller_set_color_preset(&white);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

// ========================================
// LED 關閉測試
// ========================================

void test_led_controller_off(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 關閉 LED
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    
    int result = led_controller_off();
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

// ========================================
// LED 清理測試
// ========================================

void test_led_controller_deinit(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 清理 LED - 先關閉
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    
    // 然後清理 GPIO
    hal_gpio_deinit_ExpectAndReturn(17, 0);
    hal_gpio_deinit_ExpectAndReturn(18, 0);
    hal_gpio_deinit_ExpectAndReturn(19, 0);
    
    int result = led_controller_deinit();
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

// ========================================
// 完整流程測試
// ========================================

void test_led_controller_full_flow(void)
{
    // 1. 初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    int result = led_controller_init(&test_led_config);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 2. 設定顏色
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_HIGH, 0);
    result = led_controller_set_color(255, 255, 255);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 3. 關閉
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    result = led_controller_off();
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 4. 清理
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    hal_gpio_deinit_ExpectAndReturn(17, 0);
    hal_gpio_deinit_ExpectAndReturn(18, 0);
    hal_gpio_deinit_ExpectAndReturn(19, 0);
    result = led_controller_deinit();
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}
