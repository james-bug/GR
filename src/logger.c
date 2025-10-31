/**
 * @file logger.c
 * @brief Logger 系統實作
 * @version 1.0.0
 */

#define _GNU_SOURCE  // 需要這個才能使用 vsyslog

#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// ========================================
// 私有變數
// ========================================

static bool logger_initialized = false;
static char logger_ident[64] = "gaming";
static log_level_t current_log_level = LOG_LEVEL_INFO;
static log_target_t current_log_target = LOG_TARGET_CONSOLE;

// ========================================
// 私有函數
// ========================================

/**
 * @brief 將 log_level_t 轉換為 syslog priority
 */
static int log_level_to_syslog_priority(log_level_t level) {
    switch (level) {
        case LOG_LEVEL_ERROR:
            return LOG_ERR;
        case LOG_LEVEL_WARN:      // ← 使用 LOG_LEVEL_WARN
            return LOG_WARNING;
        case LOG_LEVEL_INFO:
            return LOG_INFO;
        case LOG_LEVEL_DEBUG:
            return LOG_DEBUG;
        default:
            return LOG_INFO;
    }
}

/**
 * @brief 取得當前時間字串
 */
static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * @brief 輸出日誌到 console
 */
static void log_to_console(log_level_t level, const char *fmt, va_list args) {
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    
    const char *level_str = logger_level_string(level);
    
    fprintf(stderr, "[%s] [%s] ", timestamp, level_str);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

/**
 * @brief 輸出日誌到 syslog
 */
static void log_to_syslog(log_level_t level, const char *fmt, va_list args) {
    int priority = log_level_to_syslog_priority(level);
    vsyslog(priority, fmt, args);
}

// ========================================
// 公開 API 實作
// ========================================

int logger_init(const char *ident, log_level_t level, log_target_t target) {
    // 驗證參數
    if (level < LOG_LEVEL_ERROR || level > LOG_LEVEL_DEBUG) {
        return GAMING_ERROR_INVALID_PARAM;
    }
    
    if (target < LOG_TARGET_SYSLOG || target > LOG_TARGET_BOTH) {
        return GAMING_ERROR_INVALID_PARAM;
    }
    
    // 設定識別字
    if (ident != NULL) {
        strncpy(logger_ident, ident, sizeof(logger_ident) - 1);
        logger_ident[sizeof(logger_ident) - 1] = '\0';
    } else {
        strcpy(logger_ident, "gaming");
    }
    
    // 設定日誌等級和目標
    current_log_level = level;
    current_log_target = target;
    
    // 如果需要 syslog,開啟它
    if (target == LOG_TARGET_SYSLOG || target == LOG_TARGET_BOTH) {
        openlog(logger_ident, LOG_PID | LOG_CONS, LOG_USER);
    }
    
    logger_initialized = true;
    
    return GAMING_OK;
}

void logger_cleanup(void) {
    if (!logger_initialized) {
        return;
    }
    
    // 如果有開啟 syslog,關閉它
    if (current_log_target == LOG_TARGET_SYSLOG || 
        current_log_target == LOG_TARGET_BOTH) {
        closelog();
    }
    
    logger_initialized = false;
}

int logger_set_level(log_level_t level) {
    if (level < LOG_LEVEL_ERROR || level > LOG_LEVEL_DEBUG) {
        return GAMING_ERROR_INVALID_PARAM;
    }
    
    current_log_level = level;
    return GAMING_OK;
}

log_level_t logger_get_level(void) {
    return current_log_level;
}

int logger_set_target(log_target_t target) {
    if (target < LOG_TARGET_SYSLOG || target > LOG_TARGET_BOTH) {
        return GAMING_ERROR_INVALID_PARAM;
    }
    
    // 如果從不需要 syslog 改為需要,開啟它
    if ((current_log_target == LOG_TARGET_CONSOLE) &&
        (target == LOG_TARGET_SYSLOG || target == LOG_TARGET_BOTH)) {
        openlog(logger_ident, LOG_PID | LOG_CONS, LOG_USER);
    }
    
    // 如果從需要 syslog 改為不需要,關閉它
    if ((current_log_target == LOG_TARGET_SYSLOG || 
         current_log_target == LOG_TARGET_BOTH) &&
        (target == LOG_TARGET_CONSOLE)) {
        closelog();
    }
    
    current_log_target = target;
    return GAMING_OK;
}

log_target_t logger_get_target(void) {
    return current_log_target;
}

bool logger_should_log(log_level_t level) {
    if (!logger_initialized) {
        return false;
    }
    
    // 等級數字越大,越詳細
    // ERROR=3, WARN=2, INFO=1, DEBUG=0
    // 如果設定為 INFO,則只輸出 ERROR, WARN, INFO
    return (level >= current_log_level);
}

void logger_log(log_level_t level, const char *fmt, ...) {
    if (!logger_initialized || !logger_should_log(level)) {
        return;
    }
    
    va_list args;
    
    // 輸出到 console
    if (current_log_target == LOG_TARGET_CONSOLE || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_console(level, fmt, args);
        va_end(args);
    }
    
    // 輸出到 syslog
    if (current_log_target == LOG_TARGET_SYSLOG || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_syslog(level, fmt, args);
        va_end(args);
    }
}

void logger_error(const char *fmt, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_ERROR)) {
        return;
    }
    
    va_list args;
    
    if (current_log_target == LOG_TARGET_CONSOLE || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_console(LOG_LEVEL_ERROR, fmt, args);
        va_end(args);
    }
    
    if (current_log_target == LOG_TARGET_SYSLOG || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_syslog(LOG_LEVEL_ERROR, fmt, args);
        va_end(args);
    }
}

void logger_warning(const char *fmt, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_WARN)) {  // ← 使用 LOG_LEVEL_WARN
        return;
    }
    
    va_list args;
    
    if (current_log_target == LOG_TARGET_CONSOLE || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_console(LOG_LEVEL_WARN, fmt, args);  // ← 使用 LOG_LEVEL_WARN
        va_end(args);
    }
    
    if (current_log_target == LOG_TARGET_SYSLOG || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_syslog(LOG_LEVEL_WARN, fmt, args);  // ← 使用 LOG_LEVEL_WARN
        va_end(args);
    }
}

void logger_info(const char *fmt, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_INFO)) {
        return;
    }
    
    va_list args;
    
    if (current_log_target == LOG_TARGET_CONSOLE || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_console(LOG_LEVEL_INFO, fmt, args);
        va_end(args);
    }
    
    if (current_log_target == LOG_TARGET_SYSLOG || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_syslog(LOG_LEVEL_INFO, fmt, args);
        va_end(args);
    }
}

void logger_debug(const char *fmt, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_DEBUG)) {
        return;
    }
    
    va_list args;
    
    if (current_log_target == LOG_TARGET_CONSOLE || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_console(LOG_LEVEL_DEBUG, fmt, args);
        va_end(args);
    }
    
    if (current_log_target == LOG_TARGET_SYSLOG || 
        current_log_target == LOG_TARGET_BOTH) {
        va_start(args, fmt);
        log_to_syslog(LOG_LEVEL_DEBUG, fmt, args);
        va_end(args);
    }
}

const char* logger_level_string(log_level_t level) {
    switch (level) {
        case LOG_LEVEL_ERROR:
            return "ERROR";
        case LOG_LEVEL_WARN:       // ← 使用 LOG_LEVEL_WARN
            return "WARNING";      // ← 但顯示為 "WARNING"
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

void logger_flush(void) {
    // Console 輸出立即刷新
    fflush(stderr);
    
    // syslog 不需要手動刷新
}
