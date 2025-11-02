/**
 * @file socket_helper.h
 * @brief Socket 通訊輔助工具
 * @version 1.0.0
 * 
 * 提供 Socket 通訊的輔助函數
 * 包含 Unix domain socket 和 TCP socket 的基本操作
 */

#ifndef SOCKET_HELPER_H
#define SOCKET_HELPER_H

#include "gaming_common.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

// ========================================
// Socket 類型定義
// ========================================

typedef enum {
    SOCKET_TYPE_UNIX = 0,    // Unix domain socket
    SOCKET_TYPE_TCP  = 1,    // TCP socket
} socket_type_t;

// ========================================
// Socket 配置
// ========================================

// 預設超時時間 (秒)
#define SOCKET_DEFAULT_TIMEOUT 5

// 預設緩衝區大小
#define SOCKET_DEFAULT_BUFFER_SIZE 4096

// 預設連接佇列長度
#define SOCKET_DEFAULT_BACKLOG 5

// ========================================
// Socket Helper 公開函數
// ========================================

/**
 * @brief 建立 Unix domain socket
 * 
 * @param path Socket 檔案路徑
 * @return >= 0 Socket 檔案描述符
 * @return < 0 建立失敗
 */
int socket_helper_create_unix(const char *path);

/**
 * @brief 建立 TCP socket (server)
 * 
 * @param port 監聽埠號
 * @param backlog 連接佇列長度
 * @return >= 0 Socket 檔案描述符
 * @return < 0 建立失敗
 */
int socket_helper_create_tcp_server(int port, int backlog);

/**
 * @brief 連接到 Unix domain socket
 * 
 * @param path Socket 檔案路徑
 * @return >= 0 Socket 檔案描述符
 * @return < 0 連接失敗
 */
int socket_helper_connect_unix(const char *path);

/**
 * @brief 連接到 TCP socket (client)
 * 
 * @param host 主機位址
 * @param port 埠號
 * @return >= 0 Socket 檔案描述符
 * @return < 0 連接失敗
 */
int socket_helper_connect_tcp(const char *host, int port);

/**
 * @brief 設置 socket 超時時間
 * 
 * @param sockfd Socket 檔案描述符
 * @param timeout_sec 超時時間(秒)
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int socket_helper_set_timeout(int sockfd, int timeout_sec);

/**
 * @brief 設置 socket 為非阻塞模式
 * 
 * @param sockfd Socket 檔案描述符
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int socket_helper_set_nonblocking(int sockfd);

/**
 * @brief 設置 socket 重用地址
 * 
 * @param sockfd Socket 檔案描述符
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int socket_helper_set_reuseaddr(int sockfd);

/**
 * @brief 發送資料
 * 
 * @param sockfd Socket 檔案描述符
 * @param data 資料指標
 * @param len 資料長度
 * @return >= 0 實際發送的位元組數
 * @return < 0 發送失敗
 */
ssize_t socket_helper_send(int sockfd, const void *data, size_t len);

/**
 * @brief 接收資料
 * 
 * @param sockfd Socket 檔案描述符
 * @param buffer 接收緩衝區
 * @param len 緩衝區大小
 * @return >= 0 實際接收的位元組數
 * @return < 0 接收失敗
 */
ssize_t socket_helper_recv(int sockfd, void *buffer, size_t len);

/**
 * @brief 關閉 socket
 * 
 * @param sockfd Socket 檔案描述符
 */
void socket_helper_close(int sockfd);

/**
 * @brief 檢查 socket 是否可讀
 * 
 * @param sockfd Socket 檔案描述符
 * @param timeout_ms 超時時間(毫秒)
 * @return true 可讀
 * @return false 不可讀或超時
 */
bool socket_helper_is_readable(int sockfd, int timeout_ms);

/**
 * @brief 檢查 socket 是否可寫
 * 
 * @param sockfd Socket 檔案描述符
 * @param timeout_ms 超時時間(毫秒)
 * @return true 可寫
 * @return false 不可寫或超時
 */
bool socket_helper_is_writable(int sockfd, int timeout_ms);

#endif // SOCKET_HELPER_H
