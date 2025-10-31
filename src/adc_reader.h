/**
 * @file adc_reader.h
 * @brief ADC 讀取器 - 裝置類型判定核心模組
 * @version 1.0.0
 * 
 * 負責讀取 ADC 值並判定裝置類型 (Client/Server)
 * 提供快取機制以避免重複讀取
 */

#ifndef ADC_READER_H
#define ADC_READER_H

#include "gaming_common.h"

// ========================================
// ADC Reader 配置
// ========================================

// ADC 設備路徑（從 gaming_common.h 引用）
// #define DEVICE_ADC "/dev/ADC"

// ADC 閾值（從 gaming_common.h 引用）
// #define ADC_THRESHOLD_CLIENT_SERVER 512

// ========================================
// ADC Reader 錯誤碼
// ========================================
#define ADC_READER_OK                0
#define ADC_READER_ERROR            -1
#define ADC_READER_ERROR_IO         -2
#define ADC_READER_ERROR_NOT_INIT   -3

// ========================================
// ADC Reader 公開函數
// ========================================

/**
 * @brief 初始化 ADC 讀取器
 * 
 * 初始化 ADC 讀取器,準備讀取 ADC 值
 * 必須在使用其他 ADC Reader 函數前調用
 * 
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_HAL_FAILED HAL 初始化失敗
 */
int adc_reader_init(void);

/**
 * @brief 清理 ADC 讀取器
 * 
 * 釋放 ADC 讀取器資源,清除快取
 */
void adc_reader_cleanup(void);

/**
 * @brief 讀取原始 ADC 值
 * 
 * 從 ADC 設備讀取原始數值
 * 
 * @param device ADC 設備路徑,若為 NULL 則使用預設路徑 DEVICE_ADC
 * @return >= 0 ADC 原始值 (通常 0-1023)
 * @return < 0 讀取失敗
 * 
 * @example
 * int adc_value = adc_reader_read_raw(NULL);
 * if (adc_value < 0) {
 *     // 處理錯誤
 * }
 */
int adc_reader_read_raw(const char *device);

/**
 * @brief 偵測裝置類型
 * 
 * 根據 ADC 值判定裝置類型:
 * - ADC < ADC_THRESHOLD_CLIENT_SERVER (512) → Client 裝置
 * - ADC >= ADC_THRESHOLD_CLIENT_SERVER (512) → Server 裝置
 * 
 * @return DEVICE_TYPE_CLIENT Client 裝置 (原 Travel Router)
 * @return DEVICE_TYPE_SERVER Server 裝置 (原 Home Router)
 * @return DEVICE_TYPE_UNKNOWN 偵測失敗
 * 
 * @example
 * device_type_t type = adc_reader_detect_device_type();
 * if (type == DEVICE_TYPE_CLIENT) {
 *     printf("This is a Client device\n");
 * }
 */
device_type_t adc_reader_detect_device_type(void);

/**
 * @brief 快取裝置類型
 * 
 * 將偵測到的裝置類型存入快取
 * 快取存放於記憶體中,重啟後會清除
 * 
 * @param type 要快取的裝置類型
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_INVALID_PARAM 無效的裝置類型
 * 
 * @note 通常由 adc_reader_detect_device_type() 自動調用
 */
int adc_reader_cache_device_type(device_type_t type);

/**
 * @brief 取得快取的裝置類型
 * 
 * 從快取讀取裝置類型,避免重複讀取 ADC
 * 若快取為空,則返回 DEVICE_TYPE_UNKNOWN
 * 
 * @return DEVICE_TYPE_CLIENT 快取的 Client 類型
 * @return DEVICE_TYPE_SERVER 快取的 Server 類型
 * @return DEVICE_TYPE_UNKNOWN 快取為空或未初始化
 * 
 * @example
 * device_type_t cached_type = adc_reader_get_cached_device_type();
 * if (cached_type == DEVICE_TYPE_UNKNOWN) {
 *     // 需要重新偵測
 *     cached_type = adc_reader_detect_device_type();
 * }
 */
device_type_t adc_reader_get_cached_device_type(void);

/**
 * @brief 清除快取的裝置類型
 * 
 * 清除記憶體中的裝置類型快取
 * 下次調用 adc_reader_get_cached_device_type() 將返回 DEVICE_TYPE_UNKNOWN
 */
void adc_reader_clear_cache(void);

/**
 * @brief 取得裝置類型字串
 * 
 * 將裝置類型轉換為可讀的字串
 * 
 * @param type 裝置類型
 * @return "Client" Client 裝置
 * @return "Server" Server 裝置
 * @return "Unknown" 未知類型
 * 
 * @example
 * device_type_t type = adc_reader_detect_device_type();
 * printf("Device type: %s\n", adc_reader_get_type_string(type));
 */
const char* adc_reader_get_type_string(device_type_t type);

#endif // ADC_READER_H
