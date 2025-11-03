/**
 * @file hal_real.c
 * @brief Real Hardware Abstraction Layer Implementation
 * 
 * This file implements real hardware operations for OpenWrt devices.
 * Implements the hal_ops_t interface defined in hal_interface.h
 * 
 * @author Gaming System Team
 * @date 2025-11-03
 * @version 1.1 (Fixed to match hal_interface.h)
 */

#include "../hal_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

/* GPIO sysfs paths */
#define GPIO_SYSFS_PATH "/sys/class/gpio"
#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"

/* ADC device path */
#define ADC_DEVICE_PATH "/dev/ADC"

/* Debug output */
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[HAL Real] " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

/* ============================================================================
 * GPIO Helper Functions
 * ========================================================================== */

/**
 * @brief Export GPIO pin to userspace
 */
static int gpio_export(int pin) {
    int fd;
    char buf[16];
    
    fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to open GPIO export: %s\n", strerror(errno));
        return -1;
    }
    
    snprintf(buf, sizeof(buf), "%d", pin);
    if (write(fd, buf, strlen(buf)) < 0) {
        // Pin might already be exported, not necessarily an error
        if (errno != EBUSY) {
            fprintf(stderr, "[HAL Real] Failed to export GPIO %d: %s\n", pin, strerror(errno));
            close(fd);
            return -1;
        }
        DEBUG_PRINT("GPIO %d already exported", pin);
    }
    
    close(fd);
    return 0;
}

/**
 * @brief Unexport GPIO pin from userspace
 */
static int gpio_unexport(int pin) {
    int fd;
    char buf[16];
    
    fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to open GPIO unexport: %s\n", strerror(errno));
        return -1;
    }
    
    snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, strlen(buf));
    close(fd);
    
    return 0;
}

/**
 * @brief Set GPIO direction
 */
static int gpio_set_direction(int pin, const char *direction) {
    int fd;
    char path[64];
    
    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/direction", pin);
    
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to set GPIO %d direction: %s\n", 
                pin, strerror(errno));
        return -1;
    }
    
    if (write(fd, direction, strlen(direction)) < 0) {
        fprintf(stderr, "[HAL Real] Failed to write GPIO %d direction: %s\n",
                pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    DEBUG_PRINT("GPIO %d direction set to %s", pin, direction);
    return 0;
}

/* ============================================================================
 * GPIO Operations - Matching hal_interface.h
 * ========================================================================== */

/**
 * @brief Initialize GPIO pin
 * 
 * @param pin GPIO pin number
 * @param direction HAL_GPIO_DIR_INPUT or HAL_GPIO_DIR_OUTPUT
 * @return 0 on success, -1 on failure
 */
static int hal_real_gpio_init(int pin, hal_gpio_dir_t direction) {
    DEBUG_PRINT("Initializing GPIO %d as %s", pin, 
                direction == HAL_GPIO_DIR_OUTPUT ? "output" : "input");
    
    if (gpio_export(pin) < 0) {
        return -1;
    }
    
    // Wait a bit for sysfs to be ready
    usleep(100000); // 100ms
    
    const char *dir_str = (direction == HAL_GPIO_DIR_OUTPUT) ? "out" : "in";
    if (gpio_set_direction(pin, dir_str) < 0) {
        gpio_unexport(pin);
        return -1;
    }
    
    return 0;
}

/**
 * @brief Deinitialize GPIO pin
 * 
 * @param pin GPIO pin number
 * @return 0 on success, -1 on failure
 */
static int hal_real_gpio_deinit(int pin) {
    DEBUG_PRINT("Deinitializing GPIO %d", pin);
    return gpio_unexport(pin);
}

/**
 * @brief Read GPIO pin value
 * 
 * @param pin GPIO pin number
 * @return GPIO value (0 or 1) on success, -1 on failure
 */
static int hal_real_gpio_read(int pin) {
    int fd;
    char path[64];
    char value;
    
    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/value", pin);
    
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to open GPIO %d for reading: %s\n",
                pin, strerror(errno));
        return -1;
    }
    
    if (read(fd, &value, 1) != 1) {
        fprintf(stderr, "[HAL Real] Failed to read GPIO %d: %s\n",
                pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    
    int result = (value == '0') ? 0 : 1;
    DEBUG_PRINT("GPIO %d read: %d", pin, result);
    return result;
}

/**
 * @brief Write GPIO pin value
 * 
 * @param pin GPIO pin number
 * @param value HAL_GPIO_LOW or HAL_GPIO_HIGH
 * @return 0 on success, -1 on failure
 */
static int hal_real_gpio_write(int pin, hal_gpio_value_t value) {
    int fd;
    char path[64];
    char buf[2];
    
    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/value", pin);
    
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to open GPIO %d for writing: %s\n",
                pin, strerror(errno));
        return -1;
    }
    
    buf[0] = (value == HAL_GPIO_HIGH) ? '1' : '0';
    buf[1] = '\0';
    
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "[HAL Real] Failed to write GPIO %d: %s\n",
                pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    DEBUG_PRINT("GPIO %d write: %d", pin, value);
    return 0;
}

/**
 * @brief Set GPIO interrupt edge
 * 
 * @param pin GPIO pin number
 * @param edge "none", "rising", "falling", or "both"
 * @return 0 on success, -1 on failure
 */
static int hal_real_gpio_set_edge(int pin, const char *edge) {
    int fd;
    char path[64];
    
    DEBUG_PRINT("Setting GPIO %d edge to %s", pin, edge);
    
    snprintf(path, sizeof(path), GPIO_SYSFS_PATH "/gpio%d/edge", pin);
    
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to open GPIO %d edge: %s\n",
                pin, strerror(errno));
        return -1;
    }
    
    if (write(fd, edge, strlen(edge)) < 0) {
        fprintf(stderr, "[HAL Real] Failed to set GPIO %d edge: %s\n",
                pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    DEBUG_PRINT("GPIO %d edge set to %s", pin, edge);
    return 0;
}

/* ============================================================================
 * ADC Operations
 * ========================================================================== */

/**
 * @brief Read raw ADC value
 * 
 * @param device ADC device path (NULL to use default)
 * @return ADC value on success, -1 on failure
 * 
 * TODO: Implement actual ADC reading when hardware is available
 */
static int hal_real_adc_read(const char *device) {
    int fd;
    unsigned short value;
    ssize_t bytes_read;
    
    DEBUG_PRINT("Reading ADC from device: %s", device ? device : ADC_DEVICE_PATH);
    
    const char *adc_path = device ? device : ADC_DEVICE_PATH;
    
    fd = open(adc_path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "[HAL Real] Failed to open ADC device %s: %s\n",
                adc_path, strerror(errno));
        fprintf(stderr, "[HAL Real] ADC reading not available - hardware not ready\n");
        return -1;
    }
    
    bytes_read = read(fd, &value, sizeof(value));
    close(fd);
    
    if (bytes_read != sizeof(value)) {
        fprintf(stderr, "[HAL Real] Failed to read ADC value: %s\n", strerror(errno));
        return -1;
    }
    
    DEBUG_PRINT("ADC value read: %u", value);
    return (int)value;
}

/* ============================================================================
 * PWM Operations (Software PWM via GPIO)
 * ========================================================================== */

/**
 * @brief Initialize PWM on a GPIO pin
 * 
 * Note: This is software PWM via GPIO toggling.
 * For hardware PWM, use /sys/class/pwm interface.
 * 
 * @param pin GPIO pin number
 * @param frequency PWM frequency in Hz
 * @return 0 on success, -1 on failure
 */
static int hal_real_pwm_init(int pin, int frequency) {
    DEBUG_PRINT("Initializing PWM on GPIO %d (freq=%d Hz)", pin, frequency);
    
    // Initialize GPIO as output
    if (hal_real_gpio_init(pin, HAL_GPIO_DIR_OUTPUT) < 0) {
        return -1;
    }
    
    // Software PWM implementation would require threading or timer
    // For now, just initialize the pin
    fprintf(stderr, "[HAL Real] Software PWM not fully implemented\n");
    fprintf(stderr, "[HAL Real] Using simple on/off control instead\n");
    return 0;
}

/**
 * @brief Set PWM duty cycle
 * 
 * @param pin GPIO pin number
 * @param duty_percent Duty cycle in percent (0-100)
 * @return 0 on success, -1 on failure
 */
static int hal_real_pwm_set_duty(int pin, int duty_percent) {
    DEBUG_PRINT("Setting PWM duty cycle on GPIO %d to %d%%", pin, duty_percent);
    
    // Clamp to 0-100
    if (duty_percent < 0) duty_percent = 0;
    if (duty_percent > 100) duty_percent = 100;
    
    // For software PWM, this would adjust the timing
    // For now, just set GPIO high if duty_percent > 50, low otherwise
    hal_gpio_value_t value = (duty_percent > 50) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    return hal_real_gpio_write(pin, value);
}

/**
 * @brief Deinitialize PWM on a GPIO pin
 * 
 * @param pin GPIO pin number
 * @return 0 on success, -1 on failure
 */
static int hal_real_pwm_deinit(int pin) {
    DEBUG_PRINT("Deinitializing PWM on GPIO %d", pin);
    
    // Set GPIO low and cleanup
    hal_real_gpio_write(pin, HAL_GPIO_LOW);
    return hal_real_gpio_deinit(pin);
}

/* ============================================================================
 * System Information
 * ========================================================================== */

/**
 * @brief Get HAL implementation name
 * 
 * @return Implementation name string
 */
static const char* hal_real_get_impl_name(void) {
    return "Real Hardware HAL";
}

/* ============================================================================
 * HAL Operations Structure
 * ========================================================================== */

/**
 * @brief Real hardware HAL operations
 * 
 * This structure provides all hardware operations for real devices.
 * Must match the hal_ops_t structure defined in hal_interface.h
 */
static hal_ops_t hal_real_ops = {
    .gpio_init = hal_real_gpio_init,
    .gpio_deinit = hal_real_gpio_deinit,
    .gpio_read = hal_real_gpio_read,
    .gpio_write = hal_real_gpio_write,
    .gpio_set_edge = hal_real_gpio_set_edge,
    .adc_read = hal_real_adc_read,
    .pwm_init = hal_real_pwm_init,
    .pwm_set_duty = hal_real_pwm_set_duty,
    .pwm_deinit = hal_real_pwm_deinit,
    .get_impl_name = hal_real_get_impl_name,
};

/* ============================================================================
 * Public Interface
 * ========================================================================== */

/**
 * @brief Get real hardware HAL operations
 * 
 * This function is called by hal_init.c to get the HAL operations structure.
 * 
 * @return Pointer to HAL operations structure
 */
hal_ops_t* hal_get_real_ops(void) {
    // Check if GPIO sysfs is accessible
    struct stat st;
    if (stat(GPIO_SYSFS_PATH, &st) != 0) {
        fprintf(stderr, "[HAL Real] GPIO sysfs not available: %s\n", strerror(errno));
        fprintf(stderr, "[HAL Real] Make sure kernel has GPIO sysfs support\n");
        return NULL;
    }
    
    DEBUG_PRINT("Real hardware HAL initialized successfully");
    return &hal_real_ops;
}
