/**
 * @file logger.h
 * @brief 統一日誌系統 - syslog 整合
 * @version 1.0.0
 * 
 * 提供統一的日誌介面,整合 syslog 和 console 輸出
 * 支援不同的日誌等級和格式化輸出
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "gaming_common.h"
#include <stdarg.h>

// ========================================
// 日誌等級定義 (從 gaming_common.h 引用)
// ========================================
// LOG_LEVEL_ERROR   0
// LOG_LEVEL_WARNING 1
// LOG_LEVEL_INFO    2
// LOG_LEVEL_DEBUG   3

// ========================================
// 日誌配置
// ========================================

// 預設日誌等級
#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL LOG_LEVEL_INFO
#endif

// 最大日誌訊息長度
#define LOG_MAX_MESSAGE_LEN 512

// syslog 識別字
#define LOG_IDENT "gaming-system"

// ========================================
// 日誌輸出目標
// ========================================
typedef enum {
    LOG_TARGET_SYSLOG   = 0x01,  // 輸出到 syslog
    LOG_TARGET_CONSOLE  = 0x02,  // 輸出到 console (stdout/stderr)
    LOG_TARGET_BOTH     = 0x03,  // 同時輸出到 syslog 和 console
} log_target_t;

// ========================================
// Logger 公開函數
// ========================================

/**
 * @brief 初始化 Logger 系統
 * 
 * 初始化 syslog 連接和日誌系統
 * 
 * @param ident syslog 識別字,若為 NULL 則使用預設 LOG_IDENT
 * @param level 日誌等級 (LOG_LEVEL_ERROR/WARNING/INFO/DEBUG)
 * @param target 輸出目標 (LOG_TARGET_SYSLOG/CONSOLE/BOTH)
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 * 
 * @example
 * logger_init("gaming-client", LOG_LEVEL_DEBUG, LOG_TARGET_BOTH);
 */
int logger_init(const char *ident, int level, log_target_t target);

/**
 * @brief 清理 Logger 系統
 * 
 * 關閉 syslog 連接並清理資源
 */
void logger_cleanup(void);

/**
 * @brief 設置日誌等級
 * 
 * 動態調整日誌輸出等級
 * 
 * @param level 新的日誌等級
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_INVALID_PARAM 無效等級
 */
int logger_set_level(int level);

/**
 * @brief 取得目前日誌等級
 * 
 * @return 目前的日誌等級
 */
int logger_get_level(void);

/**
 * @brief 設置日誌輸出目標
 * 
 * @param target 輸出目標
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_INVALID_PARAM 無效目標
 */
int logger_set_target(log_target_t target);

/**
 * @brief 取得目前日誌輸出目標
 * 
 * @return 目前的輸出目標
 */
log_target_t logger_get_target(void);

// ========================================
// 日誌輸出函數
// ========================================

/**
 * @brief 輸出日誌訊息
 * 
 * 根據日誌等級和目標設定輸出訊息
 * 
 * @param level 日誌等級
 * @param format 格式化字串 (printf 風格)
 * @param ... 變數參數
 * 
 * @example
 * logger_log(LOG_LEVEL_INFO, "Device type: %d", device_type);
 */
void logger_log(int level, const char *format, ...);

/**
 * @brief 輸出 ERROR 等級日誌
 * 
 * @param format 格式化字串
 * @param ... 變數參數
 */
void logger_error(const char *format, ...);

/**
 * @brief 輸出 WARNING 等級日誌
 * 
 * @param format 格式化字串
 * @param ... 變數參數
 */
void logger_warning(const char *format, ...);

/**
 * @brief 輸出 INFO 等級日誌
 * 
 * @param format 格式化字串
 * @param ... 變數參數
 */
void logger_info(const char *format, ...);

/**
 * @brief 輸出 DEBUG 等級日誌
 * 
 * @param format 格式化字串
 * @param ... 變數參數
 */
void logger_debug(const char *format, ...);

// ========================================
// 便利巨集
// ========================================

/**
 * @brief 帶檔案名和行號的 ERROR 日誌
 */
#define LOG_ERROR(fmt, ...) \
    logger_error("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @brief 帶檔案名和行號的 WARNING 日誌
 */
#define LOG_WARNING(fmt, ...) \
    logger_warning("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @brief 帶檔案名和行號的 INFO 日誌
 */
#define LOG_INFO(fmt, ...) \
    logger_info("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * @brief 帶檔案名和行號的 DEBUG 日誌
 */
#define LOG_DEBUG(fmt, ...) \
    logger_debug("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

// ========================================
// 輔助函數
// ========================================

/**
 * @brief 取得日誌等級字串
 * 
 * @param level 日誌等級
 * @return "ERROR"/"WARNING"/"INFO"/"DEBUG"/"UNKNOWN"
 */
const char* logger_level_string(int level);

/**
 * @brief 檢查是否應該輸出該等級的日誌
 * 
 * @param level 要檢查的日誌等級
 * @return true 應該輸出
 * @return false 不應該輸出
 */
bool logger_should_log(int level);

/**
 * @brief 刷新日誌緩衝區
 * 
 * 強制將緩衝的日誌訊息立即輸出
 */
void logger_flush(void);

#endif // LOGGER_H
