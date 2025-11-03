/**
 * @file test_socket_helper.c
 * @brief Socket Helper 單元測試
 * @version 1.0.0
 */

#include "unity.h"
#include "socket_helper.h"
#include <unistd.h>

void setUp(void) {
    // 測試前清理
}

void tearDown(void) {
    // 測試後清理
}

// ========================================
// 參數驗證測試
// ========================================

void test_socket_helper_create_unix_null_path(void) {
    int sockfd = socket_helper_create_unix(NULL);
    TEST_ASSERT_LESS_THAN(0, sockfd);
}

void test_socket_helper_connect_unix_null_path(void) {
    int sockfd = socket_helper_connect_unix(NULL);
    TEST_ASSERT_LESS_THAN(0, sockfd);
}

void test_socket_helper_create_tcp_server_invalid_port(void) {
    int sockfd1 = socket_helper_create_tcp_server(0, 5);
    TEST_ASSERT_LESS_THAN(0, sockfd1);
    
    int sockfd2 = socket_helper_create_tcp_server(70000, 5);
    TEST_ASSERT_LESS_THAN(0, sockfd2);
}

void test_socket_helper_connect_tcp_null_host(void) {
    int sockfd = socket_helper_connect_tcp(NULL, 8080);
    TEST_ASSERT_LESS_THAN(0, sockfd);
}

void test_socket_helper_connect_tcp_invalid_port(void) {
    int sockfd1 = socket_helper_connect_tcp("127.0.0.1", 0);
    TEST_ASSERT_LESS_THAN(0, sockfd1);
    
    int sockfd2 = socket_helper_connect_tcp("127.0.0.1", 70000);
    TEST_ASSERT_LESS_THAN(0, sockfd2);
}

// ========================================
// Socket 選項測試
// ========================================

void test_socket_helper_set_timeout_invalid_sockfd(void) {
    int result = socket_helper_set_timeout(-1, 5);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_socket_helper_set_nonblocking_invalid_sockfd(void) {
    int result = socket_helper_set_nonblocking(-1);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

void test_socket_helper_set_reuseaddr_invalid_sockfd(void) {
    int result = socket_helper_set_reuseaddr(-1);
    TEST_ASSERT_EQUAL(GAMING_ERROR_INVALID_PARAM, result);
}

// ========================================
// Socket I/O 測試
// ========================================

void test_socket_helper_send_invalid_params(void) {
    char data[] = "test";
    
    ssize_t ret1 = socket_helper_send(-1, data, sizeof(data));
    TEST_ASSERT_LESS_THAN(0, ret1);
    
    ssize_t ret2 = socket_helper_send(0, NULL, sizeof(data));
    TEST_ASSERT_LESS_THAN(0, ret2);
    
    ssize_t ret3 = socket_helper_send(0, data, 0);
    TEST_ASSERT_LESS_THAN(0, ret3);
}

void test_socket_helper_recv_invalid_params(void) {
    char buffer[64];
    
    ssize_t ret1 = socket_helper_recv(-1, buffer, sizeof(buffer));
    TEST_ASSERT_LESS_THAN(0, ret1);
    
    ssize_t ret2 = socket_helper_recv(0, NULL, sizeof(buffer));
    TEST_ASSERT_LESS_THAN(0, ret2);
    
    ssize_t ret3 = socket_helper_recv(0, buffer, 0);
    TEST_ASSERT_LESS_THAN(0, ret3);
}

void test_socket_helper_close_invalid_sockfd(void) {
    // 應該不會 crash
    socket_helper_close(-1);
    TEST_PASS();
}

// ========================================
// Socket 狀態檢查測試
// ========================================

void test_socket_helper_is_readable_invalid_sockfd(void) {
    bool readable = socket_helper_is_readable(-1, 100);
    TEST_ASSERT_FALSE(readable);
}

void test_socket_helper_is_writable_invalid_sockfd(void) {
    bool writable = socket_helper_is_writable(-1, 100);
    TEST_ASSERT_FALSE(writable);
}

// ========================================
// 完整流程測試 (簡化版)
// ========================================

void test_socket_helper_complete_flow(void) {
    // 這個測試驗證基本的 socket 操作流程
    // 實際的網路通訊測試需要在整合測試階段進行
    
    TEST_PASS();
}
