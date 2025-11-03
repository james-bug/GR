/**
 * @file adc_reader.c
 * @brief ADC 讀取器實作
 * @version 1.0.0
 */

#include "adc_reader.h"
#include "hal_interface.h"
#include <stdio.h>
#include <string.h>

// ========================================
// 內部狀態
// ========================================

// ADC Reader 初始化狀態
static bool adc_reader_initialized = false;

// 快取的裝置類型
static device_type_t cached_device_type = DEVICE_TYPE_UNKNOWN;

// ========================================
// 內部函數
// ========================================

/**
 * @brief 檢查是否已初始化
 * @return true 已初始化
 * @return false 未初始化
 */
static bool is_initialized(void) {
    return adc_reader_initialized;
}

// ========================================
// 公開函數實作
// ========================================

int adc_reader_init(void) {
    if (is_initialized()) {
        #ifdef DEBUG
        printf("[ADC Reader] Already initialized\n");
        #endif
        return GAMING_OK;
    }

    // 檢查 HAL 是否可用
    if (hal_ops == NULL) {
        fprintf(stderr, "[ADC Reader] HAL not initialized\n");
        return GAMING_ERROR_HAL_FAILED;
    }

    // 初始化狀態
    adc_reader_initialized = true;
    cached_device_type = DEVICE_TYPE_UNKNOWN;

    #ifdef DEBUG
    printf("[ADC Reader] Initialized successfully\n");
    #endif

    return GAMING_OK;
}

void adc_reader_cleanup(void) {
    if (!is_initialized()) {
        return;
    }

    // 清除快取
    cached_device_type = DEVICE_TYPE_UNKNOWN;
    adc_reader_initialized = false;

    #ifdef DEBUG
    printf("[ADC Reader] Cleaned up\n");
    #endif
}

int adc_reader_read_raw(const char *device) {
    if (!is_initialized()) {
        fprintf(stderr, "[ADC Reader] Not initialized\n");
        return ADC_READER_ERROR_NOT_INIT;
    }

    // 使用預設設備路徑
    const char *adc_device = (device != NULL) ? device : DEVICE_ADC;

    // 透過 HAL 讀取 ADC
    if (hal_ops == NULL || hal_ops->adc_read == NULL) {
        fprintf(stderr, "[ADC Reader] HAL adc_read not available\n");
        return ADC_READER_ERROR;
    }

    int adc_value = hal_ops->adc_read(adc_device);
    
    if (adc_value < 0) {
        fprintf(stderr, "[ADC Reader] Failed to read ADC from %s\n", adc_device);
        return ADC_READER_ERROR_IO;
    }

    #ifdef DEBUG
    printf("[ADC Reader] Read ADC value: %d from %s\n", adc_value, adc_device);
    #endif

    return adc_value;
}

device_type_t adc_reader_detect_device_type(void) {
    if (!is_initialized()) {
        fprintf(stderr, "[ADC Reader] Not initialized\n");
        return DEVICE_TYPE_UNKNOWN;
    }

    // 讀取 ADC 值
    int adc_value = adc_reader_read_raw(NULL);
    
    if (adc_value < 0) {
        fprintf(stderr, "[ADC Reader] Failed to read ADC for device type detection\n");
        return DEVICE_TYPE_UNKNOWN;
    }

    // 根據 ADC 閾值判定裝置類型
    device_type_t device_type;
    
    if (adc_value < ADC_THRESHOLD_CLIENT_SERVER) {
        device_type = DEVICE_TYPE_CLIENT;
        #ifdef DEBUG
        printf("[ADC Reader] Detected Client device (ADC=%d < %d)\n", 
               adc_value, ADC_THRESHOLD_CLIENT_SERVER);
        #endif
    } else {
        device_type = DEVICE_TYPE_SERVER;
        #ifdef DEBUG
        printf("[ADC Reader] Detected Server device (ADC=%d >= %d)\n", 
               adc_value, ADC_THRESHOLD_CLIENT_SERVER);
        #endif
    }

    // 自動快取結果
    adc_reader_cache_device_type(device_type);

    return device_type;
}

int adc_reader_cache_device_type(device_type_t type) {
    if (!is_initialized()) {
        fprintf(stderr, "[ADC Reader] Not initialized\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }

    // 驗證裝置類型
    if (type != DEVICE_TYPE_CLIENT && type != DEVICE_TYPE_SERVER) {
        fprintf(stderr, "[ADC Reader] Invalid device type: %d\n", type);
        return GAMING_ERROR_INVALID_PARAM;
    }

    cached_device_type = type;

    #ifdef DEBUG
    printf("[ADC Reader] Cached device type: %s\n", 
           adc_reader_get_type_string(type));
    #endif

    return GAMING_OK;
}

device_type_t adc_reader_get_cached_device_type(void) {
    if (!is_initialized()) {
        #ifdef DEBUG
        printf("[ADC Reader] Not initialized, returning UNKNOWN\n");
        #endif
        return DEVICE_TYPE_UNKNOWN;
    }

    #ifdef DEBUG
    if (cached_device_type != DEVICE_TYPE_UNKNOWN) {
        printf("[ADC Reader] Returning cached type: %s\n", 
               adc_reader_get_type_string(cached_device_type));
    } else {
        printf("[ADC Reader] Cache is empty, returning UNKNOWN\n");
    }
    #endif

    return cached_device_type;
}

void adc_reader_clear_cache(void) {
    if (!is_initialized()) {
        return;
    }

    cached_device_type = DEVICE_TYPE_UNKNOWN;

    #ifdef DEBUG
    printf("[ADC Reader] Cache cleared\n");
    #endif
}

const char* adc_reader_get_type_string(device_type_t type) {
    switch (type) {
        case DEVICE_TYPE_CLIENT:
            return "Client";
        case DEVICE_TYPE_SERVER:
            return "Server";
        case DEVICE_TYPE_UNKNOWN:
        default:
            return "Unknown";
    }
}
