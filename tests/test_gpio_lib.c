#include "unity.h"
#include "mock_hal_interface.h"
#include "hal_interface.h"       
#include "gaming_common.h"
#include "gpio_lib.h"

// 為測試提供 hal_ops 的定義
hal_ops_t test_hal_ops_instance;
hal_ops_t *hal_ops = NULL;  // 定義全域變數（在測試中提供）

void setUp(void)
{
    // 設置 hal_ops 指向測試實例，指向 mock 函數
    test_hal_ops_instance.gpio_init = hal_gpio_init;
    test_hal_ops_instance.gpio_deinit = hal_gpio_deinit;
    test_hal_ops_instance.gpio_read = hal_gpio_read;
    test_hal_ops_instance.gpio_write = hal_gpio_write;
    test_hal_ops_instance.gpio_set_edge = hal_gpio_set_edge;
    test_hal_ops_instance.adc_read = hal_adc_read;
    test_hal_ops_instance.pwm_init = hal_pwm_init;
    test_hal_ops_instance.pwm_set_duty = hal_pwm_set_duty;
    test_hal_ops_instance.pwm_deinit = hal_pwm_deinit;
    test_hal_ops_instance.get_impl_name = hal_get_impl_name;
    
    hal_ops = &test_hal_ops_instance;
}

void tearDown(void)
{
}

// ========================================
// GPIO Init 測試
// ========================================

void test_gpio_init_output_success(void)
{
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    
    int result = gpio_lib_init_output(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_init_input_success(void)
{
    hal_gpio_init_ExpectAndReturn(16, HAL_GPIO_DIR_INPUT, 0);
    
    int result = gpio_lib_init_input(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_init_output_fail(void)
{
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, -1);
    
    int result = gpio_lib_init_output(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO Read 測試
// ========================================

void test_gpio_read_high(void)
{
    hal_gpio_read_ExpectAndReturn(16, HAL_GPIO_HIGH);
    
    int value = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(HAL_GPIO_HIGH, value);
}

void test_gpio_read_low(void)
{
    hal_gpio_read_ExpectAndReturn(16, HAL_GPIO_LOW);
    
    int value = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(HAL_GPIO_LOW, value);
}

void test_gpio_read_fail(void)
{
    hal_gpio_read_ExpectAndReturn(16, -1);
    
    int value = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, value);
}

// ========================================
// GPIO Write 測試
// ========================================

void test_gpio_write_high(void)
{
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    
    int result = gpio_lib_write(17, 1);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_write_low(void)
{
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    
    int result = gpio_lib_write(17, 0);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_write_fail(void)
{
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, -1);
    
    int result = gpio_lib_write(17, 1);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO Toggle 測試
// ========================================

void test_gpio_toggle_low_to_high(void)
{
    hal_gpio_read_ExpectAndReturn(17, HAL_GPIO_LOW);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    
    int result = gpio_lib_toggle(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_toggle_high_to_low(void)
{
    hal_gpio_read_ExpectAndReturn(17, HAL_GPIO_HIGH);
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    
    int result = gpio_lib_toggle(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_toggle_read_fail(void)
{
    hal_gpio_read_ExpectAndReturn(17, -1);
    
    int result = gpio_lib_toggle(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}
