/**
 * @file config_parser.h
 * @brief UCI 配置解析器
 * @version 1.0.0
 * 
 * 提供 UCI (Unified Configuration Interface) 配置文件的解析功能
 * 支援讀取和設置 OpenWrt UCI 配置
 */

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stddef.h>
#include "gaming_common.h"

// ========================================
// UCI 配置路徑
// ========================================

// Gaming 系統主配置
#define UCI_CONFIG_GAMING "gaming"

// Gaming Client 配置
#define UCI_CONFIG_GAMING_CLIENT "gaming-client"

// Gaming Server 配置
#define UCI_CONFIG_GAMING_SERVER "gaming-server"

// ========================================
// 配置選項定義
// ========================================

// 通用選項
#define UCI_OPTION_ENABLED      "enabled"
#define UCI_OPTION_LOG_LEVEL    "log_level"
#define UCI_OPTION_DEVICE_TYPE  "device_type"

// Client 選項
#define UCI_OPTION_VPN_ENABLED  "vpn_enabled"
#define UCI_OPTION_VPN_SOCKET   "vpn_socket"
#define UCI_OPTION_BUTTON_PIN   "button_pin"

// Server 選項
#define UCI_OPTION_PS5_DETECTION "ps5_detection"
#define UCI_OPTION_WEBSOCKET_PORT "websocket_port"
#define UCI_OPTION_CEC_DEVICE    "cec_device"

// LED 選項
#define UCI_OPTION_LED_ENABLED  "led_enabled"
#define UCI_OPTION_LED_PIN_R    "led_pin_r"
#define UCI_OPTION_LED_PIN_G    "led_pin_g"
#define UCI_OPTION_LED_PIN_B    "led_pin_b"

// ========================================
// Config Parser 公開函數
// ========================================

/**
 * @brief 初始化配置解析器
 * 
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int config_parser_init(void);

/**
 * @brief 清理配置解析器
 */
void config_parser_cleanup(void);

/**
 * @brief 讀取字串配置
 * 
 * @param config_name 配置文件名稱 (如 "gaming")
 * @param section 配置區段
 * @param option 配置選項
 * @param buffer 輸出緩衝區
 * @param buffer_size 緩衝區大小
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_NOT_FOUND 選項不存在
 * @return GAMING_ERROR 其他錯誤
 */
int config_parser_get_string(const char *config_name,
                              const char *section,
                              const char *option,
                              char *buffer,
                              size_t buffer_size);

/**
 * @brief 讀取整數配置
 * 
 * @param config_name 配置文件名稱
 * @param section 配置區段
 * @param option 配置選項
 * @param value 輸出值指標
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_NOT_FOUND 選項不存在
 * @return GAMING_ERROR 其他錯誤
 */
int config_parser_get_int(const char *config_name,
                          const char *section,
                          const char *option,
                          int *value);

/**
 * @brief 讀取布林配置
 * 
 * @param config_name 配置文件名稱
 * @param section 配置區段
 * @param option 配置選項
 * @param value 輸出值指標
 * @return GAMING_OK 成功
 * @return GAMING_ERROR_NOT_FOUND 選項不存在
 * @return GAMING_ERROR 其他錯誤
 */
int config_parser_get_bool(const char *config_name,
                           const char *section,
                           const char *option,
                           bool *value);

/**
 * @brief 設置字串配置
 * 
 * @param config_name 配置文件名稱
 * @param section 配置區段
 * @param option 配置選項
 * @param value 要設置的值
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int config_parser_set_string(const char *config_name,
                              const char *section,
                              const char *option,
                              const char *value);

/**
 * @brief 設置整數配置
 * 
 * @param config_name 配置文件名稱
 * @param section 配置區段
 * @param option 配置選項
 * @param value 要設置的值
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int config_parser_set_int(const char *config_name,
                          const char *section,
                          const char *option,
                          int value);

/**
 * @brief 設置布林配置
 * 
 * @param config_name 配置文件名稱
 * @param section 配置區段
 * @param option 配置選項
 * @param value 要設置的值
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int config_parser_set_bool(const char *config_name,
                           const char *section,
                           const char *option,
                           bool value);

/**
 * @brief 提交配置變更
 * 
 * 將記憶體中的配置變更寫入 UCI
 * 
 * @param config_name 配置文件名稱
 * @return GAMING_OK 成功
 * @return GAMING_ERROR 失敗
 */
int config_parser_commit(const char *config_name);

#endif // CONFIG_PARSER_H
