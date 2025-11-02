#include "gpio_lib.h"
#include <stdio.h>
#include <errno.h>

// ========================================
// 全域變數
// ========================================

// hal_ops 在 hal_init.c 中定義
extern hal_ops_t *hal_ops;

// ========================================
// GPIO 初始化函數
// ========================================

int gpio_lib_init_output(int pin) {
    if (!hal_ops) {
        fprintf(stderr, "GPIO lib not initialized (HAL is NULL)\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    int ret = hal_ops->gpio_init(pin, HAL_GPIO_DIR_OUTPUT);
    if (ret < 0) {
        fprintf(stderr, "Failed to init GPIO%d as output: %d\n", pin, ret);
        return GAMING_ERROR_HAL_FAILED;
    }
    
    #ifdef DEBUG
    printf("GPIO%d initialized as output\n", pin);
    #endif
    
    return GAMING_OK;
}

int gpio_lib_init_input(int pin) {
    if (!hal_ops) {
        fprintf(stderr, "GPIO lib not initialized (HAL is NULL)\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    int ret = hal_ops->gpio_init(pin, HAL_GPIO_DIR_INPUT);
    if (ret < 0) {
        fprintf(stderr, "Failed to init GPIO%d as input: %d\n", pin, ret);
        return GAMING_ERROR_HAL_FAILED;
    }
    
    #ifdef DEBUG
    printf("GPIO%d initialized as input\n", pin);
    #endif
    
    return GAMING_OK;
}

int gpio_lib_init_input_irq(int pin, const char *edge) {
    if (!hal_ops) {
        fprintf(stderr, "GPIO lib not initialized (HAL is NULL)\n");
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    if (!edge) {
        fprintf(stderr, "Invalid edge parameter\n");
        return GAMING_ERROR_INVALID_PARAM;
    }
    
    // 先初始化為輸入
    int ret = gpio_lib_init_input(pin);
    if (ret != GAMING_OK) {
        return ret;
    }
    
    // 設定中斷邊緣
    if (hal_ops->gpio_set_edge) {
        ret = hal_ops->gpio_set_edge(pin, edge);
        if (ret < 0) {
            fprintf(stderr, "Failed to set GPIO%d edge: %d\n", pin, ret);
            return GAMING_ERROR_HAL_FAILED;
        }
    }
    
    return GAMING_OK;
}

// ========================================
// GPIO 操作函數
// ========================================

int gpio_lib_read(int pin) {
    if (!hal_ops) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    int value = hal_ops->gpio_read(pin);
    if (value < 0) {
        fprintf(stderr, "Failed to read GPIO%d: %d\n", pin, value);
        return GAMING_ERROR_HAL_FAILED;
    }
    
    return value;
}

int gpio_lib_write(int pin, int value) {
    if (!hal_ops) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    hal_gpio_value_t hal_value = value ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    int ret = hal_ops->gpio_write(pin, hal_value);
    if (ret < 0) {
        fprintf(stderr, "Failed to write GPIO%d: %d\n", pin, ret);
        return GAMING_ERROR_HAL_FAILED;
    }
    
    return GAMING_OK;
}

int gpio_lib_toggle(int pin) {
    // 先讀取當前值
    int current = gpio_lib_read(pin);
    if (current < 0) {
        return current;  // 返回錯誤碼
    }
    
    // 寫入反轉的值
    return gpio_lib_write(pin, !current);
}

// ========================================
// GPIO 清理函數
// ========================================

int gpio_lib_cleanup(int pin) {
    if (!hal_ops) {
        return GAMING_ERROR_NOT_INITIALIZED;
    }
    
    if (hal_ops->gpio_deinit) {
        int ret = hal_ops->gpio_deinit(pin);
        if (ret < 0) {
            fprintf(stderr, "Failed to cleanup GPIO%d: %d\n", pin, ret);
            return GAMING_ERROR_HAL_FAILED;
        }
    }
    
    return GAMING_OK;
}
