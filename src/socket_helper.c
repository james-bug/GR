/**
 * @file socket_helper.c
 * @brief Socket 通訊輔助工具實作
 * @version 1.0.0
 */

#include "socket_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <arpa/inet.h>

// ========================================
// Unix Socket 函數
// ========================================

int socket_helper_create_unix(const char *path) {
    if (path == NULL) {
        return -1;
    }

    // 建立 socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 設置 socket 地址
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    // 刪除舊的 socket 檔案
    unlink(path);

    // 綁定
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    // 監聽
    if (listen(sockfd, SOCKET_DEFAULT_BACKLOG) < 0) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int socket_helper_connect_unix(const char *path) {
    if (path == NULL) {
        return -1;
    }

    // 建立 socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 設置 socket 地址
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    // 連接
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// ========================================
// TCP Socket 函數
// ========================================

int socket_helper_create_tcp_server(int port, int backlog) {
    if (port <= 0 || port > 65535) {
        return -1;
    }

    // 建立 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 設置 socket 重用地址
    socket_helper_set_reuseaddr(sockfd);

    // 設置 socket 地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // 綁定
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    // 監聽
    int listen_backlog = (backlog > 0) ? backlog : SOCKET_DEFAULT_BACKLOG;
    if (listen(sockfd, listen_backlog) < 0) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int socket_helper_connect_tcp(const char *host, int port) {
    if (host == NULL || port <= 0 || port > 65535) {
        return -1;
    }

    // 建立 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 設置 socket 地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    // 連接
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// ========================================
// Socket 選項設置
// ========================================

int socket_helper_set_timeout(int sockfd, int timeout_sec) {
    if (sockfd < 0) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    struct timeval timeout;
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt(SO_RCVTIMEO)");
        return GAMING_ERROR;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt(SO_SNDTIMEO)");
        return GAMING_ERROR;
    }

    return GAMING_OK;
}

int socket_helper_set_nonblocking(int sockfd) {
    if (sockfd < 0) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl(F_GETFL)");
        return GAMING_ERROR;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL)");
        return GAMING_ERROR;
    }

    return GAMING_OK;
}

int socket_helper_set_reuseaddr(int sockfd) {
    if (sockfd < 0) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
        return GAMING_ERROR;
    }

    return GAMING_OK;
}

// ========================================
// Socket I/O 函數
// ========================================

ssize_t socket_helper_send(int sockfd, const void *data, size_t len) {
    if (sockfd < 0 || data == NULL || len == 0) {
        return -1;
    }

    return send(sockfd, data, len, 0);
}

ssize_t socket_helper_recv(int sockfd, void *buffer, size_t len) {
    if (sockfd < 0 || buffer == NULL || len == 0) {
        return -1;
    }

    return recv(sockfd, buffer, len, 0);
}

void socket_helper_close(int sockfd) {
    if (sockfd >= 0) {
        close(sockfd);
    }
}

// ========================================
// Socket 狀態檢查
// ========================================

bool socket_helper_is_readable(int sockfd, int timeout_ms) {
    if (sockfd < 0) {
        return false;
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
    
    return (ret > 0 && FD_ISSET(sockfd, &readfds));
}

bool socket_helper_is_writable(int sockfd, int timeout_ms) {
    if (sockfd < 0) {
        return false;
    }

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sockfd, &writefds);

    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(sockfd + 1, NULL, &writefds, NULL, &timeout);
    
    return (ret > 0 && FD_ISSET(sockfd, &writefds));
}
