/**
 * @file logger.h
 * @brief Logger 系統 - 統一日誌介面
 * @version 1.0.0
 * 
 * 提供統一的日誌系統,支援輸出到 syslog 和 console
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "gaming_common.h"
#include <syslog.h>
#include <stdbool.h>

// ========================================
// 日誌目標定義
// ========================================

typedef enum {
    LOG_TARGET_SYSLOG = 0,   ///< 只輸出到 syslog
    LOG_TARGET_CONSOLE = 1,  ///< 只輸出到 console (stderr)
    LOG_TARGET_BOTH = 2,     ///< 同時輸出到 syslog 和 console
} log_target_t;

// ========================================
// 初始化與清理
// ========================================

/**
 * @brief 初始化日誌系統
 * 
 * @param ident 識別字串 (用於 syslog),NULL 則使用預設值 "gaming"
 * @param level 初始日誌等級
 * @param target 日誌輸出目標
 * @return GAMING_OK 成功, GAMING_ERROR_INVALID_PARAM 參數錯誤
 */
int logger_init(const char *ident, log_level_t level, log_target_t target);

/**
 * @brief 清理日誌系統
 * 
 * 關閉 syslog 連接
 */
void logger_cleanup(void);

// ========================================
// 日誌等級控制
// ========================================

/**
 * @brief 設定日誌等級
 * 
 * @param level 新的日誌等級
 * @return GAMING_OK 成功, GAMING_ERROR_INVALID_PARAM 參數錯誤
 */
int logger_set_level(log_level_t level);

/**
 * @brief 取得目前日誌等級
 * 
 * @return 目前的日誌等級
 */
log_level_t logger_get_level(void);

/**
 * @brief 設定日誌輸出目標
 * 
 * @param target 新的輸出目標
 * @return GAMING_OK 成功, GAMING_ERROR_INVALID_PARAM 參數錯誤
 */
int logger_set_target(log_target_t target);

/**
 * @brief 取得目前日誌輸出目標
 * 
 * @return 目前的輸出目標
 */
log_target_t logger_get_target(void);

/**
 * @brief 檢查是否應該輸出指定等級的日誌
 * 
 * @param level 要檢查的日誌等級
 * @return true 應該輸出, false 不應輸出
 */
bool logger_should_log(log_level_t level);

// ========================================
// 日誌輸出函數
// ========================================

/**
 * @brief 通用日誌輸出函數
 * 
 * @param level 日誌等級
 * @param fmt printf 格式字串
 * @param ... 可變參數
 */
void logger_log(log_level_t level, const char *fmt, ...) 
    __attribute__((format(printf, 2, 3)));

/**
 * @brief 輸出 ERROR 等級日誌
 * 
 * @param fmt printf 格式字串
 * @param ... 可變參數
 */
void logger_error(const char *fmt, ...) 
    __attribute__((format(printf, 1, 2)));

/**
 * @brief 輸出 WARNING 等級日誌
 * 
 * @param fmt printf 格式字串
 * @param ... 可變參數
 */
void logger_warning(const char *fmt, ...) 
    __attribute__((format(printf, 1, 2)));

/**
 * @brief 輸出 INFO 等級日誌
 * 
 * @param fmt printf 格式字串
 * @param ... 可變參數
 */
void logger_info(const char *fmt, ...) 
    __attribute__((format(printf, 1, 2)));

/**
 * @brief 輸出 DEBUG 等級日誌
 * 
 * @param fmt printf 格式字串
 * @param ... 可變參數
 */
void logger_debug(const char *fmt, ...) 
    __attribute__((format(printf, 1, 2)));

// ========================================
// 輔助函數
// ========================================

/**
 * @brief 取得日誌等級的字串表示
 * 
 * @param level 日誌等級
 * @return 等級字串 ("ERROR", "WARNING", "INFO", "DEBUG", "UNKNOWN")
 */
const char* logger_level_string(log_level_t level);

/**
 * @brief 刷新日誌緩衝區
 * 
 * 確保所有日誌都已寫入
 */
void logger_flush(void);

#endif // LOGGER_H
