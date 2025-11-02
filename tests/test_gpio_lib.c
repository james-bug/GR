/**
 * @file test_gpio_lib.c
 * @brief GPIO Library 單元測試
 * 
 * 測試 GPIO 初始化、讀寫、反轉等功能
 * 
 * @author Gaming System Team
 * @date 2024-10-30
 * @version 1.1 - 修正版 (加入 hal_ops 定義)
 */

#include "unity.h"
#include "mock_hal_interface.h"
#include "gpio_lib.h"
#include "gaming_common.h"

// ========================================
// 測試用的 HAL (必須定義!)
// ========================================
hal_ops_t test_hal_ops_instance;
hal_ops_t *hal_ops = NULL;

// ========================================
// 測試設置與拆卸
// ========================================
void setUp(void)
{
    // 設置 mock HAL
    test_hal_ops_instance.gpio_init = hal_gpio_init;
    test_hal_ops_instance.gpio_deinit = hal_gpio_deinit;
    test_hal_ops_instance.gpio_read = hal_gpio_read;
    test_hal_ops_instance.gpio_write = hal_gpio_write;
    test_hal_ops_instance.gpio_set_edge = hal_gpio_set_edge;
    
    hal_ops = &test_hal_ops_instance;
}

void tearDown(void)
{
    // 清理
}

// ========================================
// GPIO 輸出初始化測試
// ========================================

void test_gpio_lib_init_output_success(void)
{
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    
    int result = gpio_lib_init_output(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_init_output_without_hal(void)
{
    hal_ops = NULL;
    
    int result = gpio_lib_init_output(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_gpio_lib_init_output_hal_failure(void)
{
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, -1);
    
    int result = gpio_lib_init_output(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 輸入初始化測試
// ========================================

void test_gpio_lib_init_input_success(void)
{
    hal_gpio_init_ExpectAndReturn(16, HAL_GPIO_DIR_INPUT, 0);
    
    int result = gpio_lib_init_input(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_init_input_without_hal(void)
{
    hal_ops = NULL;
    
    int result = gpio_lib_init_input(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_gpio_lib_init_input_hal_failure(void)
{
    hal_gpio_init_ExpectAndReturn(16, HAL_GPIO_DIR_INPUT, -1);
    
    int result = gpio_lib_init_input(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 中斷初始化測試
// ========================================

void test_gpio_lib_init_input_irq_success(void)
{
    // 先初始化為輸入
    hal_gpio_init_ExpectAndReturn(16, HAL_GPIO_DIR_INPUT, 0);
    // 設定邊緣觸發
    hal_gpio_set_edge_ExpectAndReturn(16, "rising", 0);
    
    int result = gpio_lib_init_input_irq(16, "rising");
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_init_input_irq_without_hal(void)
{
    hal_ops = NULL;
    
    int result = gpio_lib_init_input_irq(16, "rising");
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_gpio_lib_init_input_irq_null_edge(void)
{
    int result = gpio_lib_init_input_irq(16, NULL);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_INVALID_PARAM, result);
}

void test_gpio_lib_init_input_irq_hal_failure(void)
{
    hal_gpio_init_ExpectAndReturn(16, HAL_GPIO_DIR_INPUT, -1);
    
    int result = gpio_lib_init_input_irq(16, "rising");
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 讀取測試
// ========================================

void test_gpio_lib_read_success_low(void)
{
    hal_gpio_read_ExpectAndReturn(16, HAL_GPIO_LOW);
    
    int result = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(HAL_GPIO_LOW, result);
}

void test_gpio_lib_read_success_high(void)
{
    hal_gpio_read_ExpectAndReturn(16, HAL_GPIO_HIGH);
    
    int result = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(HAL_GPIO_HIGH, result);
}

void test_gpio_lib_read_without_hal(void)
{
    hal_ops = NULL;
    
    int result = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_gpio_lib_read_hal_failure(void)
{
    hal_gpio_read_ExpectAndReturn(16, -1);
    
    int result = gpio_lib_read(16);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 寫入測試
// ========================================

void test_gpio_lib_write_success_low(void)
{
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    
    int result = gpio_lib_write(17, 0);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_write_success_high(void)
{
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    
    int result = gpio_lib_write(17, 1);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_write_without_hal(void)
{
    hal_ops = NULL;
    
    int result = gpio_lib_write(17, 1);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_gpio_lib_write_hal_failure(void)
{
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, -1);
    
    int result = gpio_lib_write(17, 1);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 反轉測試
// ========================================

void test_gpio_lib_toggle_from_low_to_high(void)
{
    // 讀取當前值 (LOW)
    hal_gpio_read_ExpectAndReturn(17, HAL_GPIO_LOW);
    // 寫入反轉值 (HIGH)
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    
    int result = gpio_lib_toggle(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_toggle_from_high_to_low(void)
{
    // 讀取當前值 (HIGH)
    hal_gpio_read_ExpectAndReturn(17, HAL_GPIO_HIGH);
    // 寫入反轉值 (LOW)
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    
    int result = gpio_lib_toggle(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_toggle_read_failure(void)
{
    // 讀取失敗
    hal_gpio_read_ExpectAndReturn(17, -1);
    
    int result = gpio_lib_toggle(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// GPIO 清理測試
// ========================================

void test_gpio_lib_cleanup_success(void)
{
    hal_gpio_deinit_ExpectAndReturn(17, 0);
    
    int result = gpio_lib_cleanup(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_cleanup_without_hal(void)
{
    hal_ops = NULL;
    
    int result = gpio_lib_cleanup(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_gpio_lib_cleanup_hal_failure(void)
{
    hal_gpio_deinit_ExpectAndReturn(17, -1);
    
    int result = gpio_lib_cleanup(17);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

// ========================================
// 完整流程測試
// ========================================

void test_gpio_lib_full_flow_output(void)
{
    // 1. 初始化為輸出
    hal_gpio_init_ExpectAndReturn(17, HAL_GPIO_DIR_OUTPUT, 0);
    int result = gpio_lib_init_output(17);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 2. 寫入 HIGH
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_HIGH, 0);
    result = gpio_lib_write(17, 1);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 3. 寫入 LOW
    hal_gpio_write_ExpectAndReturn(17, HAL_GPIO_LOW, 0);
    result = gpio_lib_write(17, 0);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 4. 清理
    hal_gpio_deinit_ExpectAndReturn(17, 0);
    result = gpio_lib_cleanup(17);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_gpio_lib_full_flow_input(void)
{
    // 1. 初始化為輸入
    hal_gpio_init_ExpectAndReturn(16, HAL_GPIO_DIR_INPUT, 0);
    int result = gpio_lib_init_input(16);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 2. 讀取值
    hal_gpio_read_ExpectAndReturn(16, HAL_GPIO_HIGH);
    int value = gpio_lib_read(16);
    TEST_ASSERT_EQUAL_INT(HAL_GPIO_HIGH, value);
    
    // 3. 清理
    hal_gpio_deinit_ExpectAndReturn(16, 0);
    result = gpio_lib_cleanup(16);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}
