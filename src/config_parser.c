/**
 * @file config_parser.c
 * @brief UCI 配置解析器實作
 * @version 1.0.0
 */

#define _POSIX_C_SOURCE 200809L

#include "config_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// ========================================
// 內部狀態
// ========================================

static bool config_parser_initialized = false;

// ========================================
// 內部輔助函數
// ========================================

/**
 * @brief 執行 UCI 命令並取得輸出
 */
static int execute_uci_command(const char *command, char *output, size_t output_size) {
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        return GAMING_ERROR;
    }

    if (fgets(output, output_size, fp) == NULL) {
        pclose(fp);
        return GAMING_ERROR_NOT_FOUND;
    }

    // 移除換行符
    size_t len = strlen(output);
    if (len > 0 && output[len - 1] == '\n') {
        output[len - 1] = '\0';
    }

    pclose(fp);
    return GAMING_OK;
}

// ========================================
// 公開函數實作
// ========================================

int config_parser_init(void) {
    if (config_parser_initialized) {
        return GAMING_OK;
    }

    config_parser_initialized = true;
    return GAMING_OK;
}

void config_parser_cleanup(void) {
    config_parser_initialized = false;
}

int config_parser_get_string(const char *config_name,
                              const char *section,
                              const char *option,
                              char *buffer,
                              size_t buffer_size) {
    if (!config_parser_initialized) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }

    if (config_name == NULL || section == NULL || option == NULL || buffer == NULL) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    char command[256];
    snprintf(command, sizeof(command), "uci get %s.%s.%s", 
             config_name, section, option);

    return execute_uci_command(command, buffer, buffer_size);
}

int config_parser_get_int(const char *config_name,
                          const char *section,
                          const char *option,
                          int *value) {
    if (value == NULL) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    char buffer[64];
    int result = config_parser_get_string(config_name, section, option, 
                                          buffer, sizeof(buffer));
    if (result != GAMING_OK) {
        return result;
    }

    *value = atoi(buffer);
    return GAMING_OK;
}

int config_parser_get_bool(const char *config_name,
                           const char *section,
                           const char *option,
                           bool *value) {
    if (value == NULL) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    char buffer[64];
    int result = config_parser_get_string(config_name, section, option, 
                                          buffer, sizeof(buffer));
    if (result != GAMING_OK) {
        return result;
    }

    *value = (strcmp(buffer, "1") == 0 || 
              strcasecmp(buffer, "true") == 0 || 
              strcasecmp(buffer, "yes") == 0);
    
    return GAMING_OK;
}

int config_parser_set_string(const char *config_name,
                              const char *section,
                              const char *option,
                              const char *value) {
    if (!config_parser_initialized) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }

    if (config_name == NULL || section == NULL || option == NULL || value == NULL) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    char command[512];
    snprintf(command, sizeof(command), "uci set %s.%s.%s='%s'", 
             config_name, section, option, value);

    int ret = system(command);
    return (ret == 0) ? GAMING_OK : GAMING_ERROR;
}

int config_parser_set_int(const char *config_name,
                          const char *section,
                          const char *option,
                          int value) {
    char value_str[32];
    snprintf(value_str, sizeof(value_str), "%d", value);
    
    return config_parser_set_string(config_name, section, option, value_str);
}

int config_parser_set_bool(const char *config_name,
                           const char *section,
                           const char *option,
                           bool value) {
    return config_parser_set_string(config_name, section, option, 
                                    value ? "1" : "0");
}

int config_parser_commit(const char *config_name) {
    if (!config_parser_initialized) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }

    if (config_name == NULL) {
        return GAMING_ERROR_INVALID_PARAM;
    }

    char command[256];
    snprintf(command, sizeof(command), "uci commit %s", config_name);

    int ret = system(command);
    return (ret == 0) ? GAMING_OK : GAMING_ERROR;
}
