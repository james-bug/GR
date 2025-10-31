/**
 * @file test_logger.c
 * @brief Logger 系統單元測試
 * @version 1.0.0
 */

#include "unity.h"
#include "logger.h"
#include <string.h>

// ========================================
// 測試前置/後置處理
// ========================================

void setUp(void) {
    // 每個測試前清理 Logger
    logger_cleanup();
}

void tearDown(void) {
    // 每個測試後清理 Logger
    logger_cleanup();
}

// ========================================
// 初始化測試
// ========================================

void test_logger_init_success(void) {
    int result = logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_logger_init_with_null_ident(void) {
    // NULL ident 應使用預設值
    int result = logger_init(NULL, LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_logger_init_invalid_level(void) {
    int result = logger_init("test-logger", 999, LOG_TARGET_CONSOLE);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_logger_init_invalid_target(void) {
    int result = logger_init("test-logger", LOG_LEVEL_INFO, 999);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_logger_init_multiple_times(void) {
    // 第一次初始化
    int result1 = logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    TEST_ASSERT_EQUAL(GAMING_OK, result1);
    
    // 第二次初始化應該也成功(冪等性)
    int result2 = logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    TEST_ASSERT_EQUAL(GAMING_OK, result2);
}

// ========================================
// 日誌等級測試
// ========================================

void test_logger_set_level_success(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    int result = logger_set_level(LOG_LEVEL_DEBUG);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
    TEST_ASSERT_EQUAL(LOG_LEVEL_DEBUG, logger_get_level());
}

void test_logger_set_level_invalid(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    int result = logger_set_level(999);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_logger_get_level(void) {
    logger_init("test-logger", LOG_LEVEL_WARN, LOG_TARGET_CONSOLE);
    
    int level = logger_get_level();
    
    TEST_ASSERT_EQUAL(LOG_LEVEL_WARN, level);
}

void test_logger_set_all_levels(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    // 測試所有合法等級
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_level(LOG_LEVEL_ERROR));
    TEST_ASSERT_EQUAL(LOG_LEVEL_ERROR, logger_get_level());
    
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_level(LOG_LEVEL_WARN));
    TEST_ASSERT_EQUAL(LOG_LEVEL_WARN, logger_get_level());
    
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_level(LOG_LEVEL_INFO));
    TEST_ASSERT_EQUAL(LOG_LEVEL_INFO, logger_get_level());
    
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_level(LOG_LEVEL_DEBUG));
    TEST_ASSERT_EQUAL(LOG_LEVEL_DEBUG, logger_get_level());
}

// ========================================
// 日誌目標測試
// ========================================

void test_logger_set_target_success(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    int result = logger_set_target(LOG_TARGET_SYSLOG);
    
    TEST_ASSERT_EQUAL(GAMING_OK, result);
    TEST_ASSERT_EQUAL(LOG_TARGET_SYSLOG, logger_get_target());
}

void test_logger_set_target_invalid(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    int result = logger_set_target(999);
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_logger_get_target(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_BOTH);
    
    log_target_t target = logger_get_target();
    
    TEST_ASSERT_EQUAL(LOG_TARGET_BOTH, target);
}

void test_logger_set_all_targets(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    // 測試所有合法目標
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_target(LOG_TARGET_SYSLOG));
    TEST_ASSERT_EQUAL(LOG_TARGET_SYSLOG, logger_get_target());
    
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_target(LOG_TARGET_CONSOLE));
    TEST_ASSERT_EQUAL(LOG_TARGET_CONSOLE, logger_get_target());
    
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_target(LOG_TARGET_BOTH));
    TEST_ASSERT_EQUAL(LOG_TARGET_BOTH, logger_get_target());
}

// ========================================
// 日誌輸出測試
// ========================================

void test_logger_error_output(void) {
    logger_init("test-logger", LOG_LEVEL_ERROR, LOG_TARGET_CONSOLE);
    
    // 這個測試主要驗證不會 crash
    logger_error("Test error message: %d", 123);
    
    TEST_PASS();
}

void test_logger_warning_output(void) {
    logger_init("test-logger", LOG_LEVEL_WARN, LOG_TARGET_CONSOLE);
    
    logger_warning("Test warning message: %s", "test");
    
    TEST_PASS();
}

void test_logger_info_output(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    logger_info("Test info message");
    
    TEST_PASS();
}

void test_logger_debug_output(void) {
    logger_init("test-logger", LOG_LEVEL_DEBUG, LOG_TARGET_CONSOLE);
    
    logger_debug("Test debug message");
    
    TEST_PASS();
}

void test_logger_log_with_format(void) {
    logger_init("test-logger", LOG_LEVEL_DEBUG, LOG_TARGET_CONSOLE);
    
    // 測試格式化輸出
    logger_log(LOG_LEVEL_INFO, "Value: %d, String: %s", 42, "test");
    
    TEST_PASS();
}

// ========================================
// 日誌過濾測試
// ========================================

void test_logger_should_log_true(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    // INFO level 應該輸出 ERROR, WARNING, INFO
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_ERROR));
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_WARN));
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_INFO));
}

void test_logger_should_log_false(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    // INFO level 不應該輸出 DEBUG
    TEST_ASSERT_FALSE(logger_should_log(LOG_LEVEL_DEBUG));
}

void test_logger_filtering_error_level(void) {
    logger_init("test-logger", LOG_LEVEL_ERROR, LOG_TARGET_CONSOLE);
    
    // ERROR level 只輸出 ERROR
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_ERROR));
    TEST_ASSERT_FALSE(logger_should_log(LOG_LEVEL_WARN));
    TEST_ASSERT_FALSE(logger_should_log(LOG_LEVEL_INFO));
    TEST_ASSERT_FALSE(logger_should_log(LOG_LEVEL_DEBUG));
}

void test_logger_filtering_debug_level(void) {
    logger_init("test-logger", LOG_LEVEL_DEBUG, LOG_TARGET_CONSOLE);
    
    // DEBUG level 輸出所有等級
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_ERROR));
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_WARN));
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_INFO));
    TEST_ASSERT_TRUE(logger_should_log(LOG_LEVEL_DEBUG));
}

// ========================================
// 輔助函數測試
// ========================================

void test_logger_level_string(void) {
    TEST_ASSERT_EQUAL_STRING("ERROR", logger_level_string(LOG_LEVEL_ERROR));
    TEST_ASSERT_EQUAL_STRING("WARNING", logger_level_string(LOG_LEVEL_WARN));
    TEST_ASSERT_EQUAL_STRING("INFO", logger_level_string(LOG_LEVEL_INFO));
    TEST_ASSERT_EQUAL_STRING("DEBUG", logger_level_string(LOG_LEVEL_DEBUG));
    TEST_ASSERT_EQUAL_STRING("UNKNOWN", logger_level_string(999));
}

void test_logger_flush(void) {
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    
    logger_info("Test message");
    logger_flush();
    
    TEST_PASS();
}

// ========================================
// 完整流程測試
// ========================================

void test_complete_logger_flow(void) {
    // 初始化
    TEST_ASSERT_EQUAL(GAMING_OK, logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE));
    
    // 輸出各等級日誌
    logger_error("Error message");
    logger_warning("Warning message");
    logger_info("Info message");
    logger_debug("Debug message (should be filtered)");
    
    // 動態調整等級
    TEST_ASSERT_EQUAL(GAMING_OK, logger_set_level(LOG_LEVEL_DEBUG));
    logger_debug("Debug message (should be visible now)");
    
    // 清理
    logger_cleanup();
    
    TEST_PASS();
}

void test_logger_after_cleanup(void) {
    // 初始化後清理
    logger_init("test-logger", LOG_LEVEL_INFO, LOG_TARGET_CONSOLE);
    logger_cleanup();
    
    // 清理後輸出應該不會 crash (但不會輸出)
    logger_info("This should not output");
    
    TEST_PASS();
}
