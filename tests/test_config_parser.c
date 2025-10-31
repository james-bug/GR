/**
 * @file test_config_parser.c
 * @brief Config Parser 單元測試
 * @version 1.0.0
 */

#include "unity.h"
#include "config_parser.h"

void setUp(void) {
    config_parser_cleanup();
}

void tearDown(void) {
    config_parser_cleanup();
}

// ========================================
// 初始化測試
// ========================================

void test_config_parser_init_success(void) {
    int result = config_parser_init();
    TEST_ASSERT_EQUAL(GAMING_OK, result);
}

void test_config_parser_init_multiple_times(void) {
    TEST_ASSERT_EQUAL(GAMING_OK, config_parser_init());
    TEST_ASSERT_EQUAL(GAMING_OK, config_parser_init());
}

// ========================================
// 字串讀取測試
// ========================================

void test_config_parser_get_string_not_initialized(void) {
    char buffer[64];
    int result = config_parser_get_string("gaming", "main", "test", 
                                          buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_config_parser_get_string_null_params(void) {
    config_parser_init();
    char buffer[64];
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, 
                      config_parser_get_string(NULL, "main", "test", buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_get_string("gaming", NULL, "test", buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_get_string("gaming", "main", NULL, buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_get_string("gaming", "main", "test", NULL, sizeof(buffer)));
}

// ========================================
// 整數讀取測試
// ========================================

void test_config_parser_get_int_null_value(void) {
    config_parser_init();
    
    int result = config_parser_get_int("gaming", "main", "test", NULL);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

// ========================================
// 布林讀取測試
// ========================================

void test_config_parser_get_bool_null_value(void) {
    config_parser_init();
    
    int result = config_parser_get_bool("gaming", "main", "test", NULL);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

// ========================================
// 設置測試
// ========================================

void test_config_parser_set_string_not_initialized(void) {
    int result = config_parser_set_string("gaming", "main", "test", "value");
    TEST_ASSERT_EQUAL(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_config_parser_set_string_null_params(void) {
    config_parser_init();
    
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_set_string(NULL, "main", "test", "value"));
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_set_string("gaming", NULL, "test", "value"));
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_set_string("gaming", "main", NULL, "value"));
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM,
                      config_parser_set_string("gaming", "main", "test", NULL));
}

// ========================================
// Commit 測試
// ========================================

void test_config_parser_commit_not_initialized(void) {
    int result = config_parser_commit("gaming");
    TEST_ASSERT_EQUAL(GAMING_ERROR_NOT_INITIALIZED, result);
}

void test_config_parser_commit_null_param(void) {
    config_parser_init();
    
    int result = config_parser_commit(NULL);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

// ========================================
// 完整流程測試
// ========================================

void test_config_parser_complete_flow(void) {
    // 初始化
    TEST_ASSERT_EQUAL(GAMING_OK, config_parser_init());
    
    // 清理
    config_parser_cleanup();
    
    TEST_PASS();
}
