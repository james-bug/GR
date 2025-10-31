#ifndef GAMING_COMMON_H
#define GAMING_COMMON_H

#include <stdint.h>
#include <stdbool.h>

// ========================================
// 版本資訊
// ========================================
#define GAMING_VERSION_MAJOR 1
#define GAMING_VERSION_MINOR 0
#define GAMING_VERSION_PATCH 0

// ========================================
// 錯誤碼定義
// ========================================
typedef enum {
    GAMING_OK = 0,
    GAMING_ERROR = -1,
    GAMING_ERROR_INVALID_PARAM = -2,
    GAMING_ERROR_NOT_INITIALIZED = -3,
    GAMING_ERROR_HAL_FAILED = -4,
    GAMING_ERROR_TIMEOUT = -5,
    GAMING_ERROR_NOT_FOUND = -6,
    GAMING_ERROR_ALREADY_EXISTS = -7,
    GAMING_ERROR_NO_MEMORY = -8,
    GAMING_ERROR_IO = -9,
} gaming_error_t;

// ========================================
// 裝置類型定義
// ========================================
typedef enum {
    DEVICE_TYPE_UNKNOWN = 0,
    DEVICE_TYPE_CLIENT = 1,    // 原 Travel Router
    DEVICE_TYPE_SERVER = 2,    // 原 Home Router
} device_type_t;

// ADC 閾值
#define ADC_THRESHOLD_CLIENT_SERVER 512

// ========================================
// PS5 狀態定義
// ========================================
typedef enum {
    PS5_STATE_UNKNOWN = 0,
    PS5_STATE_ON = 1,
    PS5_STATE_STANDBY = 2,
    PS5_STATE_OFF = 3,
} ps5_state_t;

// ========================================
// LED 顏色定義
// ========================================
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

// 預定義顏色
#define LED_COLOR_WHITE    ((led_color_t){255, 255, 255})
#define LED_COLOR_ORANGE   ((led_color_t){255, 165, 0})
#define LED_COLOR_BLACK    ((led_color_t){0, 0, 0})
#define LED_COLOR_RED      ((led_color_t){255, 0, 0})
#define LED_COLOR_GREEN    ((led_color_t){0, 255, 0})
#define LED_COLOR_BLUE     ((led_color_t){0, 0, 255})

// ========================================
// GPIO Pin 定義（從 UCI 讀取，這裡是預設值）
// ========================================
#define GPIO_PIN_BUTTON    16
#define GPIO_PIN_LED_R     17
#define GPIO_PIN_LED_G     18
#define GPIO_PIN_LED_B     19

// ========================================
// 系統路徑定義
// ========================================
#define DEVICE_ADC         "/dev/ADC"
#define DEVICE_CEC         "/dev/cec0"

#define PATH_RUN_DIR       "/var/run"
#define PATH_DEVICE_TYPE_CACHE  PATH_RUN_DIR "/gaming_device_type"
#define PATH_VPN_SOCKET    PATH_RUN_DIR "/vpn_status.sock"
#define PATH_BUTTON_SOCKET PATH_RUN_DIR "/gaming_button.sock"
#define PATH_PS5_IP_CACHE  PATH_RUN_DIR "/ps5_ip.cache"
#define PATH_PS5_MAC_CACHE PATH_RUN_DIR "/ps5_mac.txt"

// ========================================
// WebSocket 配置
// ========================================
#define WEBSOCKET_PORT     8080
#define WEBSOCKET_PATH     "/gaming"

// ========================================
// 日誌等級定義
// ========================================
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3,
} log_level_t;

// ========================================
// 通用輔助巨集
// ========================================
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif // GAMING_COMMON_H
