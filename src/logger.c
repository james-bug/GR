/**
 * @file logger.c
 * @brief Logger 系統實作
 * @version 1.0.0
 */

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

// ========================================
// 內部狀態
// ========================================

// Logger 初始化狀態
static bool logger_initialized = false;

// 目前日誌等級
static int current_log_level = DEFAULT_LOG_LEVEL;

// 日誌輸出目標
static log_target_t current_log_target = LOG_TARGET_BOTH;

// syslog 識別字
static char logger_ident[64] = LOG_IDENT;

// ========================================
// 內部函數
// ========================================

/**
 * @brief 將日誌等級轉換為 syslog 優先級
 */
static int log_level_to_syslog_priority(int level) {
    switch (level) {
        case LOG_LEVEL_ERROR:
            return LOG_ERR;
        case LOG_LEVEL_WARNING:
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
 * @brief 取得目前時間戳記字串
 */
static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * @brief 輸出到 console
 */
static void output_to_console(int level, const char *message) {
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    
    FILE *output = (level == LOG_LEVEL_ERROR) ? stderr : stdout;
    
    fprintf(output, "[%s] [%s] %s\n", 
            timestamp, 
            logger_level_string(level), 
            message);
    
    fflush(output);
}

/**
 * @brief 輸出到 syslog
 */
static void output_to_syslog(int level, const char *message) {
    int priority = log_level_to_syslog_priority(level);
    syslog(priority, "%s", message);
}

// ========================================
// 公開函數實作
// ========================================

int logger_init(const char *ident, int level, log_target_t target) {
    if (logger_initialized) {
        #ifdef DEBUG
        printf("[Logger] Already initialized\n");
        #endif
        return GAMING_OK;
    }

    // 驗證參數
    if (level < LOG_LEVEL_ERROR || level > LOG_LEVEL_DEBUG) {
        fprintf(stderr, "[Logger] Invalid log level: %d\n", level);
        return GAMING_ERROR_INVALID_PARAM;
    }

    if (target < LOG_TARGET_SYSLOG || target > LOG_TARGET_BOTH) {
        fprintf(stderr, "[Logger] Invalid log target: %d\n", target);
        return GAMING_ERROR_INVALID_PARAM;
    }

    // 設置識別字
    if (ident != NULL) {
        strncpy(logger_ident, ident, sizeof(logger_ident) - 1);
        logger_ident[sizeof(logger_ident) - 1] = '\0';
    }

    // 設置日誌等級和目標
    current_log_level = level;
    current_log_target = target;

    // 初始化 syslog (如果需要)
    if (target & LOG_TARGET_SYSLOG) {
        openlog(logger_ident, LOG_PID | LOG_CONS, LOG_DAEMON);
    }

    logger_initialized = true;

    #ifdef DEBUG
    printf("[Logger] Initialized: ident=%s, level=%s, target=%d\n",
           logger_ident, logger_level_string(level), target);
    #endif

    logger_info("Logger initialized: %s", logger_ident);

    return GAMING_OK;
}

void logger_cleanup(void) {
    if (!logger_initialized) {
        return;
    }

    logger_info("Logger shutting down");

    // 關閉 syslog
    if (current_log_target & LOG_TARGET_SYSLOG) {
        closelog();
    }

    logger_initialized = false;

    #ifdef DEBUG
    printf("[Logger] Cleaned up\n");
    #endif
}

int logger_set_level(int level) {
    if (level < LOG_LEVEL_ERROR || level > LOG_LEVEL_DEBUG) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    int old_level = current_log_level;
    current_log_level = level;

    if (logger_initialized) {
        logger_info("Log level changed: %s -> %s",
                    logger_level_string(old_level),
                    logger_level_string(level));
    }

    return GAMING_OK;
}

int logger_get_level(void) {
    return current_log_level;
}

int logger_set_target(log_target_t target) {
    if (target < LOG_TARGET_SYSLOG || target > LOG_TARGET_BOTH) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    log_target_t old_target = current_log_target;
    current_log_target = target;

    // 如果從不使用 syslog 改為使用,需要開啟
    if (!(old_target & LOG_TARGET_SYSLOG) && (target & LOG_TARGET_SYSLOG)) {
        openlog(logger_ident, LOG_PID | LOG_CONS, LOG_DAEMON);
    }

    // 如果從使用 syslog 改為不使用,需要關閉
    if ((old_target & LOG_TARGET_SYSLOG) && !(target & LOG_TARGET_SYSLOG)) {
        closelog();
    }

    if (logger_initialized) {
        logger_info("Log target changed: %d -> %d", old_target, target);
    }

    return GAMING_OK;
}

log_target_t logger_get_target(void) {
    return current_log_target;
}

void logger_log(int level, const char *format, ...) {
    if (!logger_initialized) {
        return;
    }

    if (!logger_should_log(level)) {
        return;
    }

    char message[LOG_MAX_MESSAGE_LEN];
    va_list args;

    // 格式化訊息
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // 輸出到目標
    if (current_log_target & LOG_TARGET_CONSOLE) {
        output_to_console(level, message);
    }

    if (current_log_target & LOG_TARGET_SYSLOG) {
        output_to_syslog(level, message);
    }
}

void logger_error(const char *format, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_ERROR)) {
        return;
    }

    char message[LOG_MAX_MESSAGE_LEN];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    logger_log(LOG_LEVEL_ERROR, "%s", message);
}

void logger_warning(const char *format, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_WARNING)) {
        return;
    }

    char message[LOG_MAX_MESSAGE_LEN];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    logger_log(LOG_LEVEL_WARNING, "%s", message);
}

void logger_info(const char *format, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_INFO)) {
        return;
    }

    char message[LOG_MAX_MESSAGE_LEN];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    logger_log(LOG_LEVEL_INFO, "%s", message);
}

void logger_debug(const char *format, ...) {
    if (!logger_initialized || !logger_should_log(LOG_LEVEL_DEBUG)) {
        return;
    }

    char message[LOG_MAX_MESSAGE_LEN];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    logger_log(LOG_LEVEL_DEBUG, "%s", message);
}

const char* logger_level_string(int level) {
    switch (level) {
        case LOG_LEVEL_ERROR:
            return "ERROR";
        case LOG_LEVEL_WARNING:
            return "WARNING";
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

bool logger_should_log(int level) {
    return logger_initialized && (level <= current_log_level);
}

void logger_flush(void) {
    fflush(stdout);
    fflush(stderr);
}
