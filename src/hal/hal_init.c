#include "hal_interface.h"
#include <stdio.h>
#include <string.h>

// ========================================
// 全域 HAL 操作表指標
// ========================================
hal_ops_t *hal_ops = NULL;

// ========================================
// 外部函數宣告（由 hal_real.c 和 hal_mock.c 提供）
// ========================================
#ifndef TEST
extern hal_ops_t* hal_get_real_ops(void);
#endif

#ifdef TEST
// 在測試模式下，我們不需要實際的 HAL 實作
// CMock 會自動處理
#endif

// ========================================
// HAL 初始化函數
// ========================================
int hal_init(const char *mode) {
    if (mode == NULL) {
        fprintf(stderr, "HAL init: mode is NULL\n");
        return -1;
    }
    
    #ifndef TEST
    if (strcmp(mode, "real") == 0) {
        hal_ops = hal_get_real_ops();
        printf("HAL initialized: Real Hardware\n");
        return 0;
    } else if (strcmp(mode, "mock") == 0) {
        // Mock mode - 在測試環境中，hal_ops 將由 CMock 處理
        printf("HAL initialized: Mock Hardware\n");
        return 0;
    } else {
        fprintf(stderr, "HAL init: unknown mode '%s'\n", mode);
        return -1;
    }
    #else
    // 測試模式下，不進行實際初始化
    printf("HAL init (test mode): %s\n", mode);
    return 0;
    #endif
}

// ========================================
// HAL 清理函數
// ========================================
void hal_cleanup(void) {
    hal_ops = NULL;
}
