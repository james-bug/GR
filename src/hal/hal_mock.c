
/**
 * @file hal_mock.c
 * @brief HAL Mock Implementation for Unit Testing
 * 
 * 提供硬體抽象層的模擬實作，用於單元測試環境。
 * 使用記憶體陣列模擬 GPIO、ADC、PWM 狀態。
 * 
 * @date 2025-10-30
 * @version 1.0.0
 */

#include "hal_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// ========================================
// Mock 狀態儲存
// ========================================

#define MAX_GPIO_PINS 64
#define MAX_PWM_CHANNELS 8

// GPIO 狀態
static struct {
    bool initialized;
    hal_gpio_dir_t direction;
    hal_gpio_value_t value;
    char edge[16];  // "none", "rising", "falling", "both"
} mock_gpio_state[MAX_GPIO_PINS];

// ADC 狀態
static struct {
    int value;
    bool enabled;
} mock_adc_state = {0, true};

// PWM 狀態
static struct {
    bool initialized;
    int frequency;
    int duty_percent;
} mock_pwm_state[MAX_PWM_CHANNELS];

// 統計資訊 (用於測試驗證)
static struct {
    int gpio_init_count;
    int gpio_read_count;
    int gpio_write_count;
    int adc_read_count;
    int pwm_init_count;
} mock_stats;

// ========================================
// 內部輔助函數
// ========================================

/**
 * @brief 檢查 GPIO 引腳編號是否有效
 */
static bool is_valid_pin(int pin) {
    return (pin >= 0 && pin < MAX_GPIO_PINS);
}

/**
 * @brief 檢查 PWM 通道是否有效
 */
static bool is_valid_pwm_channel(int pin) {
    return (pin >= 0 && pin < MAX_PWM_CHANNELS);
}

// ========================================
// Mock GPIO 操作實作
// ========================================

/**
 * @brief 初始化 GPIO (Mock)
 * @param pin GPIO 引腳編號 (0-63)
 * @param direction GPIO 方向 (INPUT/OUTPUT)
 * @return 0 成功, <0 失敗
 */
static int mock_gpio_init(int pin, hal_gpio_dir_t direction) {
    if (!is_valid_pin(pin)) {
        fprintf(stderr, "Mock GPIO: Invalid pin %d\n", pin);
        return -1;
    }

    mock_gpio_state[pin].initialized = true;
    mock_gpio_state[pin].direction = direction;
    mock_gpio_state[pin].value = HAL_GPIO_LOW;  // 預設為 LOW
    strcpy(mock_gpio_state[pin].edge, "none");
    
    mock_stats.gpio_init_count++;
    
    #ifdef DEBUG
    printf("Mock GPIO%d initialized as %s\n", 
           pin, direction == HAL_GPIO_DIR_OUTPUT ? "OUTPUT" : "INPUT");
    #endif
    
    return 0;
}

/**
 * @brief 清理 GPIO (Mock)
 * @param pin GPIO 引腳編號
 * @return 0 成功, <0 失敗
 */
static int mock_gpio_deinit(int pin) {
    if (!is_valid_pin(pin)) {
        return -1;
    }
    
    if (!mock_gpio_state[pin].initialized) {
        fprintf(stderr, "Mock GPIO: Pin %d not initialized\n", pin);
        return -2;
    }
    
    mock_gpio_state[pin].initialized = false;
    mock_gpio_state[pin].direction = HAL_GPIO_DIR_INPUT;
    mock_gpio_state[pin].value = HAL_GPIO_LOW;
    strcpy(mock_gpio_state[pin].edge, "none");
    
    #ifdef DEBUG
    printf("Mock GPIO%d deinitialized\n", pin);
    #endif
    
    return 0;
}

/**
 * @brief 讀取 GPIO 值 (Mock)
 * @param pin GPIO 引腳編號
 * @return GPIO 值 (0/1), <0 失敗
 */
static int mock_gpio_read(int pin) {
    if (!is_valid_pin(pin)) {
        fprintf(stderr, "Mock GPIO: Invalid pin %d\n", pin);
        return -1;
    }
    
    if (!mock_gpio_state[pin].initialized) {
        fprintf(stderr, "Mock GPIO: Pin %d not initialized\n", pin);
        return -2;
    }
    
    mock_stats.gpio_read_count++;
    
    #ifdef DEBUG
    printf("Mock GPIO%d read: %d\n", pin, mock_gpio_state[pin].value);
    #endif
    
    return mock_gpio_state[pin].value;
}

/**
 * @brief 寫入 GPIO 值 (Mock)
 * @param pin GPIO 引腳編號
 * @param value GPIO 值 (LOW/HIGH)
 * @return 0 成功, <0 失敗
 */
static int mock_gpio_write(int pin, hal_gpio_value_t value) {
    if (!is_valid_pin(pin)) {
        fprintf(stderr, "Mock GPIO: Invalid pin %d\n", pin);
        return -1;
    }
    
    if (!mock_gpio_state[pin].initialized) {
        fprintf(stderr, "Mock GPIO: Pin %d not initialized\n", pin);
        return -2;
    }
    
    if (mock_gpio_state[pin].direction != HAL_GPIO_DIR_OUTPUT) {
        fprintf(stderr, "Mock GPIO: Pin %d not configured as output\n", pin);
        return -3;
    }
    
    mock_gpio_state[pin].value = value;
    mock_stats.gpio_write_count++;
    
    #ifdef DEBUG
    printf("Mock GPIO%d write: %d\n", pin, value);
    #endif
    
    return 0;
}

/**
 * @brief 設定 GPIO 中斷邊緣 (Mock)
 * @param pin GPIO 引腳編號
 * @param edge 邊緣類型 ("none", "rising", "falling", "both")
 * @return 0 成功, <0 失敗
 */
static int mock_gpio_set_edge(int pin, const char *edge) {
    if (!is_valid_pin(pin)) {
        return -1;
    }
    
    if (!mock_gpio_state[pin].initialized) {
        fprintf(stderr, "Mock GPIO: Pin %d not initialized\n", pin);
        return -2;
    }
    
    if (!edge) {
        fprintf(stderr, "Mock GPIO: edge parameter is NULL\n");
        return -3;
    }
    
    // 驗證 edge 參數
    if (strcmp(edge, "none") != 0 && 
        strcmp(edge, "rising") != 0 && 
        strcmp(edge, "falling") != 0 && 
        strcmp(edge, "both") != 0) {
        fprintf(stderr, "Mock GPIO: Invalid edge type '%s'\n", edge);
        return -4;
    }
    
    strncpy(mock_gpio_state[pin].edge, edge, sizeof(mock_gpio_state[pin].edge) - 1);
    mock_gpio_state[pin].edge[sizeof(mock_gpio_state[pin].edge) - 1] = '\0';
    
    #ifdef DEBUG
    printf("Mock GPIO%d edge set to: %s\n", pin, edge);
    #endif
    
    return 0;
}

// ========================================
// Mock ADC 操作實作
// ========================================

/**
 * @brief 讀取 ADC 值 (Mock)
 * @param device ADC 設備路徑 (在 Mock 中被忽略)
 * @return ADC 值 (0-1023), <0 失敗
 */
static int mock_adc_read(const char *device) {
    if (!device) {
        fprintf(stderr, "Mock ADC: device parameter is NULL\n");
        return -1;
    }
    
    if (!mock_adc_state.enabled) {
        fprintf(stderr, "Mock ADC: ADC is disabled\n");
        return -2;
    }
    
    mock_stats.adc_read_count++;
    
    #ifdef DEBUG
    printf("Mock ADC read: %d\n", mock_adc_state.value);
    #endif
    
    return mock_adc_state.value;
}

// ========================================
// Mock PWM 操作實作
// ========================================

/**
 * @brief 初始化 PWM (Mock)
 * @param pin PWM 通道編號
 * @param frequency PWM 頻率 (Hz)
 * @return 0 成功, <0 失敗
 */
static int mock_pwm_init(int pin, int frequency) {
    if (!is_valid_pwm_channel(pin)) {
        fprintf(stderr, "Mock PWM: Invalid channel %d\n", pin);
        return -1;
    }
    
    if (frequency <= 0) {
        fprintf(stderr, "Mock PWM: Invalid frequency %d\n", frequency);
        return -2;
    }
    
    mock_pwm_state[pin].initialized = true;
    mock_pwm_state[pin].frequency = frequency;
    mock_pwm_state[pin].duty_percent = 0;
    
    mock_stats.pwm_init_count++;
    
    #ifdef DEBUG
    printf("Mock PWM%d initialized with frequency %d Hz\n", pin, frequency);
    #endif
    
    return 0;
}

/**
 * @brief 設定 PWM 佔空比 (Mock)
 * @param pin PWM 通道編號
 * @param duty_percent 佔空比 (0-100%)
 * @return 0 成功, <0 失敗
 */
static int mock_pwm_set_duty(int pin, int duty_percent) {
    if (!is_valid_pwm_channel(pin)) {
        fprintf(stderr, "Mock PWM: Invalid channel %d\n", pin);
        return -1;
    }
    
    if (!mock_pwm_state[pin].initialized) {
        fprintf(stderr, "Mock PWM: Channel %d not initialized\n", pin);
        return -2;
    }
    
    if (duty_percent < 0 || duty_percent > 100) {
        fprintf(stderr, "Mock PWM: Invalid duty cycle %d%%\n", duty_percent);
        return -3;
    }
    
    mock_pwm_state[pin].duty_percent = duty_percent;
    
    #ifdef DEBUG
    printf("Mock PWM%d duty set to %d%%\n", pin, duty_percent);
    #endif
    
    return 0;
}

/**
 * @brief 清理 PWM (Mock)
 * @param pin PWM 通道編號
 * @return 0 成功, <0 失敗
 */
static int mock_pwm_deinit(int pin) {
    if (!is_valid_pwm_channel(pin)) {
        return -1;
    }
    
    if (!mock_pwm_state[pin].initialized) {
        fprintf(stderr, "Mock PWM: Channel %d not initialized\n", pin);
        return -2;
    }
    
    mock_pwm_state[pin].initialized = false;
    mock_pwm_state[pin].frequency = 0;
    mock_pwm_state[pin].duty_percent = 0;
    
    #ifdef DEBUG
    printf("Mock PWM%d deinitialized\n", pin);
    #endif
    
    return 0;
}

// ========================================
// Mock 系統資訊
// ========================================

/**
 * @brief 取得 HAL 實作名稱
 * @return "mock"
 */
static const char* mock_get_impl_name(void) {
    return "mock";
}

// ========================================
// HAL 操作表 (Mock)
// ========================================

static hal_ops_t mock_hal_ops = {
    .gpio_init = mock_gpio_init,
    .gpio_deinit = mock_gpio_deinit,
    .gpio_read = mock_gpio_read,
    .gpio_write = mock_gpio_write,
    .gpio_set_edge = mock_gpio_set_edge,
    .adc_read = mock_adc_read,
    .pwm_init = mock_pwm_init,
    .pwm_set_duty = mock_pwm_set_duty,
    .pwm_deinit = mock_pwm_deinit,
    .get_impl_name = mock_get_impl_name,
};

/**
 * @brief 取得 Mock HAL 操作表
 * @return Mock HAL 操作表指標
 */
hal_ops_t* hal_get_mock_ops(void) {
    return &mock_hal_ops;
}

// ========================================
// 測試輔助函數
// ========================================

#ifdef TEST

/**
 * @brief 設定 Mock ADC 值 (測試用)
 * @param value ADC 值 (0-1023)
 */
void mock_hal_set_adc_value(int value) {
    mock_adc_state.value = value;
    #ifdef DEBUG
    printf("Mock ADC value set to: %d\n", value);
    #endif
}

/**
 * @brief 設定 Mock GPIO 值 (測試用，模擬外部輸入)
 * @param pin GPIO 引腳編號
 * @param value GPIO 值 (LOW/HIGH)
 */
void mock_hal_set_gpio_value(int pin, hal_gpio_value_t value) {
    if (!is_valid_pin(pin)) {
        fprintf(stderr, "Mock HAL: Invalid pin %d\n", pin);
        return;
    }
    
    mock_gpio_state[pin].value = value;
    
    #ifdef DEBUG
    printf("Mock GPIO%d value set to: %d (externally)\n", pin, value);
    #endif
}

/**
 * @brief 取得 Mock GPIO 值 (測試用，驗證輸出)
 * @param pin GPIO 引腳編號
 * @return GPIO 值 (0/1), <0 失敗
 */
int mock_hal_get_gpio_value(int pin) {
    if (!is_valid_pin(pin)) {
        return -1;
    }
    
    return mock_gpio_state[pin].value;
}

/**
 * @brief 取得 Mock GPIO 方向 (測試用)
 * @param pin GPIO 引腳編號
 * @return GPIO 方向, -1 失敗
 */
int mock_hal_get_gpio_direction(int pin) {
    if (!is_valid_pin(pin)) {
        return -1;
    }
    
    return mock_gpio_state[pin].direction;
}

/**
 * @brief 檢查 GPIO 是否已初始化 (測試用)
 * @param pin GPIO 引腳編號
 * @return true 已初始化, false 未初始化
 */
bool mock_hal_is_gpio_initialized(int pin) {
    if (!is_valid_pin(pin)) {
        return false;
    }
    
    return mock_gpio_state[pin].initialized;
}

/**
 * @brief 取得 GPIO edge 設定 (測試用)
 * @param pin GPIO 引腳編號
 * @return edge 字串, NULL 失敗
 */
const char* mock_hal_get_gpio_edge(int pin) {
    if (!is_valid_pin(pin)) {
        return NULL;
    }
    
    return mock_gpio_state[pin].edge;
}

/**
 * @brief 啟用/停用 Mock ADC (測試用)
 * @param enabled true 啟用, false 停用
 */
void mock_hal_set_adc_enabled(bool enabled) {
    mock_adc_state.enabled = enabled;
}

/**
 * @brief 取得 PWM 佔空比 (測試用)
 * @param pin PWM 通道編號
 * @return 佔空比 (0-100%), <0 失敗
 */
int mock_hal_get_pwm_duty(int pin) {
    if (!is_valid_pwm_channel(pin)) {
        return -1;
    }
    
    if (!mock_pwm_state[pin].initialized) {
        return -2;
    }
    
    return mock_pwm_state[pin].duty_percent;
}

/**
 * @brief 取得 PWM 頻率 (測試用)
 * @param pin PWM 通道編號
 * @return 頻率 (Hz), <0 失敗
 */
int mock_hal_get_pwm_frequency(int pin) {
    if (!is_valid_pwm_channel(pin)) {
        return -1;
    }
    
    if (!mock_pwm_state[pin].initialized) {
        return -2;
    }
    
    return mock_pwm_state[pin].frequency;
}

/**
 * @brief 取得統計資訊 (測試用)
 * @param gpio_init GPIO 初始化次數 (輸出參數)
 * @param gpio_read GPIO 讀取次數 (輸出參數)
 * @param gpio_write GPIO 寫入次數 (輸出參數)
 * @param adc_read ADC 讀取次數 (輸出參數)
 */
void mock_hal_get_stats(int *gpio_init, int *gpio_read, int *gpio_write, int *adc_read) {
    if (gpio_init) *gpio_init = mock_stats.gpio_init_count;
    if (gpio_read) *gpio_read = mock_stats.gpio_read_count;
    if (gpio_write) *gpio_write = mock_stats.gpio_write_count;
    if (adc_read) *adc_read = mock_stats.adc_read_count;
}

/**
 * @brief 重置 Mock HAL 狀態 (測試用)
 * 
 * 將所有 GPIO、ADC、PWM 狀態重置為初始值。
 * 應在每個測試案例的 setUp() 中呼叫。
 */
void mock_hal_reset(void) {
    // 重置 GPIO 狀態
    for (int i = 0; i < MAX_GPIO_PINS; i++) {
        mock_gpio_state[i].initialized = false;
        mock_gpio_state[i].direction = HAL_GPIO_DIR_INPUT;
        mock_gpio_state[i].value = HAL_GPIO_LOW;
        strcpy(mock_gpio_state[i].edge, "none");
    }
    
    // 重置 ADC 狀態
    mock_adc_state.value = 0;
    mock_adc_state.enabled = true;
    
    // 重置 PWM 狀態
    for (int i = 0; i < MAX_PWM_CHANNELS; i++) {
        mock_pwm_state[i].initialized = false;
        mock_pwm_state[i].frequency = 0;
        mock_pwm_state[i].duty_percent = 0;
    }
    
    // 重置統計
    memset(&mock_stats, 0, sizeof(mock_stats));
    
    #ifdef DEBUG
    printf("Mock HAL reset\n");
    #endif
}

#endif 







