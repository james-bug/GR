/**
 * @file test_adc_reader.c
 * @brief ADC Reader 單元測試
 * @version 1.0.0
 */

#include "unity.h"
#include "adc_reader.h"
#include "mock_hal_interface.h"

// ========================================
// 測試前置/後置處理
// ========================================

void setUp(void) {
    // 每個測試前重置 Mock
    mock_hal_reset();
    
    // 清理 ADC Reader 狀態
    adc_reader_cleanup();
}

void tearDown(void) {
    // 每個測試後清理
    adc_reader_cleanup();
}

// ========================================
// 初始化測試
// ========================================

void test_adc_reader_init_success(void) {
    // HAL 已初始化的情況
    hal_ops = (hal_ops_t*)1; // 模擬 HAL 已初始化
    
    int result = adc_reader_init();
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_adc_reader_init_without_hal(void) {
    // HAL 未初始化的情況
    hal_ops = NULL;
    
    int result = adc_reader_init();
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_HAL_FAILED, result);
}

void test_adc_reader_init_multiple_times(void) {
    hal_ops = (hal_ops_t*)1;
    
    // 第一次初始化
    int result1 = adc_reader_init();
    TEST_ASSERT_EQUAL(GAMING_OK, result1);
    
    // 第二次初始化應該也成功(冪等性)
    int result2 = adc_reader_init();
    TEST_ASSERT_EQUAL(GAMING_OK, result2);
}

// ========================================
// ADC 讀取測試
// ========================================

void test_adc_reader_read_raw_success(void) {
    // 初始化
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // Mock ADC 讀取返回 256
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 256);
    
    int adc_value = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL(256, adc_value);
}

void test_adc_reader_read_raw_custom_device(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // Mock 自定義設備路徑
    const char *custom_device = "/dev/ADC1";
    hal_adc_read_ExpectAndReturn(custom_device, 768);
    
    int adc_value = adc_reader_read_raw(custom_device);
    
    TEST_ASSERT_EQUAL(768, adc_value);
}

void test_adc_reader_read_raw_not_initialized(void) {
    // 未初始化就讀取
    int adc_value = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL(ADC_READER_ERROR_NOT_INIT, adc_value);
}

void test_adc_reader_read_raw_hal_failure(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // Mock HAL 讀取失敗
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, -1);
    
    int adc_value = adc_reader_read_raw(NULL);
    
    TEST_ASSERT_EQUAL(ADC_READER_ERROR_IO, adc_value);
}

// ========================================
// 裝置類型偵測測試
// ========================================

void test_detect_device_type_client(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // Mock ADC 值小於閾值 (512) → Client
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 256);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

void test_detect_device_type_server(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // Mock ADC 值大於等於閾值 (512) → Server
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 768);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
}

void test_detect_device_type_boundary_client(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // 邊界測試: ADC = 511 → Client
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 511);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
}

void test_detect_device_type_boundary_server(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // 邊界測試: ADC = 512 → Server
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 512);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
}

void test_detect_device_type_adc_failure(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // Mock ADC 讀取失敗
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, -1);
    
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, type);
}

void test_detect_device_type_not_initialized(void) {
    // 未初始化就偵測
    device_type_t type = adc_reader_detect_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, type);
}

// ========================================
// 快取機制測試
// ========================================

void test_cache_device_type_client(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    int result = adc_reader_cache_device_type(DEVICE_TYPE_CLIENT);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_cache_device_type_server(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    int result = adc_reader_cache_device_type(DEVICE_TYPE_SERVER);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_cache_device_type_invalid(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // 嘗試快取 UNKNOWN 類型
    int result = adc_reader_cache_device_type(DEVICE_TYPE_UNKNOWN);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_get_cached_device_type_empty(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // 未快取任何類型
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, type);
}

void test_get_cached_device_type_after_detection(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // 偵測裝置類型(會自動快取)
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 256);
    device_type_t detected_type = adc_reader_detect_device_type();
    
    // 從快取讀取
    device_type_t cached_type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, detected_type);
    TEST_ASSERT_EQUAL(detected_type, cached_type);
}

void test_clear_cache(void) {
    hal_ops = (hal_ops_t*)1;
    adc_reader_init();
    
    // 快取一個類型
    adc_reader_cache_device_type(DEVICE_TYPE_CLIENT);
    
    // 清除快取
    adc_reader_clear_cache();
    
    // 讀取應該返回 UNKNOWN
    device_type_t type = adc_reader_get_cached_device_type();
    
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, type);
}

// ========================================
// 輔助函數測試
// ========================================

void test_get_type_string_client(void) {
    const char *str = adc_reader_get_type_string(DEVICE_TYPE_CLIENT);
    TEST_ASSERT_EQUAL_STRING("Client", str);
}

void test_get_type_string_server(void) {
    const char *str = adc_reader_get_type_string(DEVICE_TYPE_SERVER);
    TEST_ASSERT_EQUAL_STRING("Server", str);
}

void test_get_type_string_unknown(void) {
    const char *str = adc_reader_get_type_string(DEVICE_TYPE_UNKNOWN);
    TEST_ASSERT_EQUAL_STRING("Unknown", str);
}

// ========================================
// 完整流程測試
// ========================================

void test_complete_flow_client_device(void) {
    // 初始化
    hal_ops = (hal_ops_t*)1;
    TEST_ASSERT_EQUAL(GAMING_OK, adc_reader_init());
    
    // 偵測裝置類型
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 200);
    device_type_t type = adc_reader_detect_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, type);
    
    // 驗證快取
    device_type_t cached_type = adc_reader_get_cached_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_CLIENT, cached_type);
    
    // 清理
    adc_reader_cleanup();
    
    // 清理後快取應該清空
    cached_type = adc_reader_get_cached_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_UNKNOWN, cached_type);
}

void test_complete_flow_server_device(void) {
    // 初始化
    hal_ops = (hal_ops_t*)1;
    TEST_ASSERT_EQUAL(GAMING_OK, adc_reader_init());
    
    // 偵測裝置類型
    hal_adc_read_ExpectAndReturn(DEVICE_ADC, 900);
    device_type_t type = adc_reader_detect_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, type);
    
    // 驗證快取
    device_type_t cached_type = adc_reader_get_cached_device_type();
    TEST_ASSERT_EQUAL(DEVICE_TYPE_SERVER, cached_type);
    
    // 清理
    adc_reader_cleanup();
}
