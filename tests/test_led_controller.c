

#include "unity.h"
#include "mock_hal_interface.h"
#include "led_controller.h"
#include "gaming_common.h"

// 測試用的 HAL
hal_ops_t test_hal_ops_instance;
hal_ops_t *hal_ops = NULL;

// 測試配置
static led_config_t test_led_config = {
    .pin_r = 17,
    .pin_g = 18,
    .pin_b = 19
};

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

void test_led_controller_init_should_fail_when_gpio_init_fails(void)
{
    // 第一個 GPIO 初始化失敗
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, -1);
    
    int result = led_controller_init(&test_led_config);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// LED 顏色控制測試
// ========================================

void test_led_set_color_should_set_white(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 設定白色（全部 HIGH）
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);  // R
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);  // G
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_HIGH, 0);  // B
    
    int result = led_set_color(255, 255, 255);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_led_set_color_should_set_red(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 設定紅色
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);  // R
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);   // G
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);   // B
    
    int result = led_set_color(255, 0, 0);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_led_set_color_preset_white(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 使用預設顏色
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_HIGH, 0);
    
    int result = led_set_color_preset(LED_COLOR_WHITE);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

// ========================================
// LED 關閉測試
// ========================================

void test_led_off_should_turn_off_all_leds(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // 關閉所有 LED
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    
    int result = led_off();
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

// ========================================
// LED 狀態指示測試
// ========================================

void test_led_set_status_client_ps5_on_should_show_white(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // Client + PS5 ON = 白色
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_HIGH, 0);
    
    int result = led_set_status(DEVICE_TYPE_CLIENT, PS5_STATE_ON);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_led_set_status_client_ps5_standby_should_show_orange(void)
{
    // 先初始化
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(18, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_init_ExpectAndReturn(19, HAL_GPIO_DIR_OUTPUT, 0);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_LOW, 0);
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);
    led_controller_init(&test_led_config);
    
    // Client + PS5 Standby = 橙色
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);  // R
    hal_gpio_write_ExpectAndReturn(18, HAL_GPIO_HIGH, 0);  // G (部分)
    hal_gpio_write_ExpectAndReturn(19, HAL_GPIO_LOW, 0);   // B
    
    int result = led_set_status(DEVICE_TYPE_CLIENT, PS5_STATE_STANDBY);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}



