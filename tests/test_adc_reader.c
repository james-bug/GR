/**
 * @file test_adc_reader.c
 * @brief ADC Reader 單元測試
 * 
 * 測試 ADC 讀取、裝置類型判定、快取機制等功能
 * 
 * @author Gaming System Team
 * @date 2024-10-30
 * @version 1.1 - 修正版
 */

#include "unity.h"
#include "mock_hal_interface.h"
#include "adc_reader.h"
#include "gaming_common.h"
#include <string.h>

// ========================================
// 測試用的 HAL (必須定義!)
// ========================================
hal_ops_t test_hal_ops_instance;
hal_ops_t *hal_ops = NULL;

// ========================================
// 測試常數
// ========================================
#define TEST_ADC_CLIENT_VALUE  256    // Client 裝置 ADC 值 (< 512)
#define TEST_ADC_SERVER_VALUE  768    // Server 裝置 ADC 值 (>= 512)
#define TEST_ADC_BOUNDARY_LOW  0      // 邊界測試: 最低值
#define TEST_ADC_BOUNDARY_HIGH 1023   // 邊界測試: 最高值

// ========================================
// 測試設置與拆卸
// ========================================
void setUp(void)
{
    // 設置 mock HAL
    test_hal_ops_instance.adc_read = hal_adc_read;
    hal_ops = &test_hal_ops_instance;
}

void tearDown(void)
{
    // 清理
}

// ========================================
// ADC 初始化測試
// ========================================

void test_adc_reader_init_success(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    
    int result = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL_INT(TEST_ADC_CLIENT_VALUE, result);
}

void test_adc_reader_init_without_hal(void)
{
    hal_ops = NULL;
    
    int result = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_adc_reader_init_multiple_times(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    int result1 = adc_reader_read_raw(NULL);
    TEST_ASSERT_EQUAL_INT(TEST_ADC_CLIENT_VALUE, result1);
    
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_SERVER_VALUE);
    int result2 = adc_reader_read_raw(NULL);
    TEST_ASSERT_EQUAL_INT(TEST_ADC_SERVER_VALUE, result2);
}

// ========================================
// ADC 讀取測試
// ========================================

void test_adc_reader_read_raw_success(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    
    int result = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL_INT(TEST_ADC_CLIENT_VALUE, result);
}

void test_adc_reader_read_raw_custom_device(void)
{
    const char *custom_device = "/dev/custom_adc";
    
    hal_adc_read_ExpectAndReturn(custom_device, TEST_ADC_SERVER_VALUE);
    
    int result = adc_reader_read_raw(custom_device);
    
    TEST_ASSERT_EQUAL_INT(TEST_ADC_SERVER_VALUE, result);
}

void test_adc_reader_read_raw_hal_failure(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, -1);
    
    int result = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL_INT(GAMING_ERROR_HAL_FAILED, result);
}

void test_adc_reader_read_raw_boundary_low(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_BOUNDARY_LOW);
    
    int result = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL_INT(TEST_ADC_BOUNDARY_LOW, result);
}

void test_adc_reader_read_raw_boundary_high(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_BOUNDARY_HIGH);
    
    int result = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL_INT(TEST_ADC_BOUNDARY_HIGH, result);
}

// ========================================
// 裝置類型偵測測試
// ========================================

void test_adc_reader_detect_client_device(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

void test_adc_reader_detect_server_device(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_SERVER_VALUE);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
}

void test_adc_reader_detect_boundary_client(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 511);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

void test_adc_reader_detect_boundary_server(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 512);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
}

void test_adc_reader_detect_adc_read_failure(void)
{
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, -1);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, type);
}

void test_adc_reader_detect_without_hal(void)
{
    hal_ops = NULL;
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, type);
}

// ========================================
// 快取機制測試
// ========================================

void test_adc_reader_cache_client_type(void)
{
    int result = adc_reader_cache_device_type(DEVICE_TYPE_CLIENT);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_adc_reader_cache_server_type(void)
{
    int result = adc_reader_cache_device_type(DEVICE_TYPE_SERVER);
    
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
}

void test_adc_reader_get_cached_client(void)
{
    // 先快取
    adc_reader_cache_device_type(DEVICE_TYPE_CLIENT);
    
    // 讀取快取
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

void test_adc_reader_get_cached_server(void)
{
    // 先快取
    adc_reader_cache_device_type(DEVICE_TYPE_SERVER);
    
    // 讀取快取
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
}

void test_adc_reader_get_cached_no_file(void)
{
    // 確保快取檔案不存在
    remove(PATH_DEVICE_TYPE_CACHE);
    
    // 當快取不存在時,會觸發 ADC 讀取
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

// ========================================
// 輔助函數測試
// ========================================

void test_adc_reader_get_cached_with_detection(void)
{
    // 清除快取
    remove(PATH_DEVICE_TYPE_CACHE);
    
    // Mock ADC 讀取
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    
    // 應該自動偵測並快取
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

void test_adc_reader_get_cached_from_cache(void)
{
    // 先快取
    adc_reader_cache_device_type(DEVICE_TYPE_SERVER);
    
    // 讀取時不應該呼叫 ADC
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
}

// ========================================
// 整合測試
// ========================================

void test_adc_reader_full_flow_client(void)
{
    // 1. 讀取 ADC
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    int adc_value = adc_reader_read_raw(NULL);
    TEST_ASSERT_EQUAL_INT(TEST_ADC_CLIENT_VALUE, adc_value);
    
    // 2. 偵測裝置類型
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_CLIENT_VALUE);
    device_type_t type = adc_reader_detect_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
    
    // 3. 快取裝置類型
    int result = adc_reader_cache_device_type(type);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 4. 從快取讀取
    device_type_t cached_type = adc_reader_get_cached_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, cached_type);
}

void test_adc_reader_full_flow_server(void)
{
    // 1. 讀取 ADC
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_SERVER_VALUE);
    int adc_value = adc_reader_read_raw(NULL);
    TEST_ASSERT_EQUAL_INT(TEST_ADC_SERVER_VALUE, adc_value);
    
    // 2. 偵測裝置類型
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, TEST_ADC_SERVER_VALUE);
    device_type_t type = adc_reader_detect_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
    
    // 3. 快取裝置類型
    int result = adc_reader_cache_device_type(type);
    TEST_ASSERT_EQUAL_INT(GAMING_OK, result);
    
    // 4. 從快取讀取
    device_type_t cached_type = adc_reader_get_cached_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, cached_type);
}
