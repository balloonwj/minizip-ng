# 进度回调和取消支持使用指南

本指南介绍如何使用 minizip-ng helper 函数的进度回调和取消功能。

## 📋 新功能概述

### 扩展函数（带 `_ex` 后缀）

新增了三个扩展函数，支持进度回调和操作取消：

1. `mz_helper_compress_file_ex()` - 压缩单个文件（带进度）
2. `mz_helper_compress_directory_ex()` - 压缩目录（带进度）
3. `mz_helper_extract_ex()` - 解压文件（带进度）

### 向后兼容性

✅ 原有函数完全保留，不受影响：
- `mz_helper_compress_file()`
- `mz_helper_compress_directory()`
- `mz_helper_extract()`

您可以根据需要选择使用简单版本或扩展版本。

---

## 🎯 进度回调函数

### 回调函数签名

```c
typedef int32_t (*mz_helper_progress_cb)(
    void *userdata,         // 用户自定义数据指针
    const char *current_file,  // 当前正在处理的文件名
    int64_t bytes_done,     // 当前文件已处理的字节数
    int64_t bytes_total,    // 当前文件总字节数
    int32_t file_index,     // 当前文件索引（0开始）
    int32_t file_count      // 文件总数
);
```

### 返回值

回调函数应该返回以下值之一：

- `MZ_HELPER_PROGRESS_CONTINUE` (0) - 继续操作
- `MZ_HELPER_PROGRESS_CANCEL` (1) - 取消操作

---

## 💡 基本使用示例

### 示例 1：简单进度显示

```c
#include "mz_zip_helper.h"
#include <stdio.h>

// 进度回调函数
int32_t progress_callback(void *userdata, const char *current_file,
                         int64_t bytes_done, int64_t bytes_total,
                         int32_t file_index, int32_t file_count) {
    int32_t percent = 0;

    if (bytes_total > 0)
        percent = (int32_t)((bytes_done * 100) / bytes_total);

    printf("\r[%d/%d] %s: %d%%",
           file_index + 1, file_count, current_file, percent);
    fflush(stdout);

    return MZ_HELPER_PROGRESS_CONTINUE;
}

int main() {
    int32_t err;

    // 压缩文件夹，显示进度
    err = mz_helper_compress_directory_ex(
        "backup.zip",           // 输出文件
        "MyDocuments",          // 要压缩的目录
        1,                      // 包含路径
        NULL,                   // 无密码
        MZ_HELPER_COMPRESS_LEVEL_DEFAULT,
        progress_callback,      // 进度回调
        NULL                    // 用户数据（这里不需要）
    );

    if (err == MZ_OK) {
        printf("\n压缩成功！\n");
    } else if (err == MZ_USER_CANCELED) {
        printf("\n操作被取消\n");
    } else {
        printf("\n压缩失败：%d\n", err);
    }

    return 0;
}
```

### 示例 2：带用户数据的进度显示

```c
#include "mz_zip_helper.h"
#include <stdio.h>

// 用户数据结构
typedef struct {
    int32_t last_percent;
    int64_t total_bytes_processed;
} progress_info;

// 进度回调函数
int32_t progress_callback(void *userdata, const char *current_file,
                         int64_t bytes_done, int64_t bytes_total,
                         int32_t file_index, int32_t file_count) {
    progress_info *info = (progress_info *)userdata;
    int32_t percent = 0;

    if (bytes_total > 0)
        percent = (int32_t)((bytes_done * 100) / bytes_total);

    // 只在百分比变化时更新显示
    if (percent != info->last_percent) {
        info->total_bytes_processed += (bytes_done - info->last_percent);
        info->last_percent = percent;

        printf("\r[%d/%d] %s: %d%% (总计: %" PRId64 " 字节)",
               file_index + 1, file_count, current_file,
               percent, info->total_bytes_processed);
        fflush(stdout);
    }

    return MZ_HELPER_PROGRESS_CONTINUE;
}

int main() {
    progress_info info = {0, 0};
    int32_t err;

    err = mz_helper_extract_ex(
        "archive.zip",
        "./extracted",
        NULL,
        1,
        progress_callback,
        &info  // 传递用户数据
    );

    printf("\n");
    return (err == MZ_OK) ? 0 : 1;
}
```

### 示例 3：支持取消操作

```c
#include "mz_zip_helper.h"
#include <stdio.h>
#include <signal.h>

// 全局取消标志
static volatile int cancel_requested = 0;

// 信号处理（Ctrl+C）
void signal_handler(int sig) {
    cancel_requested = 1;
    printf("\n正在取消操作...\n");
}

// 进度回调函数
int32_t progress_callback(void *userdata, const char *current_file,
                         int64_t bytes_done, int64_t bytes_total,
                         int32_t file_index, int32_t file_count) {
    // 检查取消标志
    if (cancel_requested)
        return MZ_HELPER_PROGRESS_CANCEL;

    int32_t percent = 0;
    if (bytes_total > 0)
        percent = (int32_t)((bytes_done * 100) / bytes_total);

    printf("\r[%d/%d] %s: %d%%",
           file_index + 1, file_count, current_file, percent);
    fflush(stdout);

    return MZ_HELPER_PROGRESS_CONTINUE;
}

int main() {
    int32_t err;

    // 设置信号处理（允许 Ctrl+C 取消）
    signal(SIGINT, signal_handler);

    printf("压缩中... (按 Ctrl+C 取消)\n");

    err = mz_helper_compress_directory_ex(
        "backup.zip",
        "LargeFolder",
        1,
        NULL,
        MZ_HELPER_COMPRESS_LEVEL_DEFAULT,
        progress_callback,
        NULL
    );

    printf("\n");

    if (err == MZ_OK) {
        printf("✓ 压缩完成\n");
    } else if (err == MZ_USER_CANCELED) {
        printf("✗ 操作已取消\n");
    } else {
        printf("✗ 错误：%d\n", err);
    }

    return 0;
}
```

### 示例 4：条件取消（达到某个百分比）

```c
int32_t progress_callback(void *userdata, const char *current_file,
                         int64_t bytes_done, int64_t bytes_total,
                         int32_t file_index, int32_t file_count) {
    int32_t percent = 0;

    if (bytes_total > 0)
        percent = (int32_t)((bytes_done * 100) / bytes_total);

    printf("\r进度: %d%%", percent);
    fflush(stdout);

    // 如果达到 50%，取消操作（用于测试）
    if (percent >= 50) {
        printf("\n测试：在 50%% 处取消\n");
        return MZ_HELPER_PROGRESS_CANCEL;
    }

    return MZ_HELPER_PROGRESS_CONTINUE;
}
```

---

## 🖥️ Windows GUI 应用程序示例

### Windows 进度对话框

```c
#include "mz_zip_helper.h"
#include <windows.h>

// Windows 应用程序的进度回调
int32_t windows_progress_callback(void *userdata, const char *current_file,
                                 int64_t bytes_done, int64_t bytes_total,
                                 int32_t file_index, int32_t file_count) {
    HWND hProgress = (HWND)userdata;  // 进度条句柄
    int32_t percent = 0;

    if (bytes_total > 0)
        percent = (int32_t)((bytes_done * 100) / bytes_total);

    // 更新进度条
    SendMessage(hProgress, PBM_SETPOS, percent, 0);

    // 更新状态文本
    char status[512];
    sprintf(status, "压缩 [%d/%d]: %s - %d%%",
            file_index + 1, file_count, current_file, percent);
    SetWindowText(GetDlgItem(GetParent(hProgress), IDC_STATUS_TEXT), status);

    // 处理消息（允许取消按钮响应）
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 检查用户是否点击了取消按钮
    if (IsDlgButtonChecked(GetParent(hProgress), IDC_CANCEL_BUTTON))
        return MZ_HELPER_PROGRESS_CANCEL;

    return MZ_HELPER_PROGRESS_CONTINUE;
}

// 使用示例
void CompressFolder(HWND hProgress) {
    int32_t err = mz_helper_compress_directory_ex(
        "C:\\backup.zip",
        "C:\\MyDocuments",
        1,
        NULL,
        MZ_HELPER_COMPRESS_LEVEL_DEFAULT,
        windows_progress_callback,
        (void*)hProgress  // 传递进度条句柄
    );

    if (err == MZ_OK) {
        MessageBox(NULL, "压缩完成！", "成功", MB_OK);
    } else if (err == MZ_USER_CANCELED) {
        MessageBox(NULL, "操作已取消", "取消", MB_OK);
    } else {
        char msg[128];
        sprintf(msg, "压缩失败，错误代码：%d", err);
        MessageBox(NULL, msg, "错误", MB_ICONERROR);
    }
}
```

---

## 🔧 API 参考

### mz_helper_compress_file_ex

压缩单个文件（带进度回调）

```c
int32_t mz_helper_compress_file_ex(
    const char *zip_path,          // 输出 ZIP 文件路径
    const char *file_path,         // 要压缩的文件路径
    const char *password,          // 密码（NULL = 无密码）
    int16_t compress_level,        // 压缩级别（0-9 或 -1）
    mz_helper_progress_cb progress_cb,  // 进度回调函数
    void *userdata                 // 用户数据
);
```

### mz_helper_compress_directory_ex

压缩目录（带进度回调）

```c
int32_t mz_helper_compress_directory_ex(
    const char *zip_path,          // 输出 ZIP 文件路径
    const char *dir_path,          // 要压缩的目录路径
    uint8_t include_path,          // 1=包含路径，0=仅内容
    const char *password,          // 密码
    int16_t compress_level,        // 压缩级别
    mz_helper_progress_cb progress_cb,  // 进度回调函数
    void *userdata                 // 用户数据
);
```

### mz_helper_extract_ex

解压 ZIP 文件（带进度回调）

```c
int32_t mz_helper_extract_ex(
    const char *zip_path,          // ZIP 文件路径
    const char *dest_dir,          // 目标目录
    const char *password,          // 密码
    uint8_t overwrite,             // 1=覆盖，0=跳过
    mz_helper_progress_cb progress_cb,  // 进度回调函数
    void *userdata                 // 用户数据
);
```

---

## 📝 注意事项

1. **线程安全**：回调函数在操作线程中调用，确保 UI 更新是线程安全的

2. **性能**：
   - 回调会频繁调用，避免在回调中执行耗时操作
   - 建议只在百分比变化时更新 UI

3. **错误处理**：
   - 返回值：`MZ_OK` = 成功，`MZ_USER_CANCELED` = 用户取消
   - 检查返回值以区分成功、取消和错误

4. **文件计数**：
   - 对于目录压缩，文件总数在开始时计算
   - 可能会有轻微延迟（扫描文件）

---

## 🎯 最佳实践

### 1. 简单进度条

```c
// 只显示百分比，性能最好
int32_t simple_progress(void *data, const char *file,
                       int64_t done, int64_t total,
                       int32_t index, int32_t count) {
    int32_t pct = (total > 0) ? (int32_t)((done * 100) / total) : 0;
    printf("\r%d%%", pct);
    return MZ_HELPER_PROGRESS_CONTINUE;
}
```

### 2. 详细进度信息

```c
// 显示文件名和进度
int32_t detailed_progress(void *data, const char *file,
                         int64_t done, int64_t total,
                         int32_t index, int32_t count) {
    printf("\r[%d/%d] %s: %" PRId64 "/%" PRId64,
           index + 1, count, file, done, total);
    return MZ_HELPER_PROGRESS_CONTINUE;
}
```

### 3. 智能更新（减少刷新）

```c
typedef struct {
    int32_t last_pct;
    clock_t last_update;
} smart_progress;

int32_t smart_callback(void *data, const char *file,
                      int64_t done, int64_t total,
                      int32_t index, int32_t count) {
    smart_progress *sp = (smart_progress *)data;
    int32_t pct = (total > 0) ? (int32_t)((done * 100) / total) : 0;
    clock_t now = clock();

    // 只在百分比变化或超过100ms时更新
    if (pct != sp->last_pct ||
        (now - sp->last_update) > CLOCKS_PER_SEC / 10) {
        printf("\r%d%%", pct);
        sp->last_pct = pct;
        sp->last_update = now;
    }

    return MZ_HELPER_PROGRESS_CONTINUE;
}
```

---

## 🚀 运行示例程序

编译后的示例程序：`mz_zip_helper_progress_example`

```bash
# 压缩文件夹，显示进度
./mz_zip_helper_progress_example -c output.zip MyFolder

# 解压文件，显示进度
./mz_zip_helper_progress_example -x archive.zip ./output

# 测试取消功能（在 50% 处自动取消）
./mz_zip_helper_progress_example -c test.zip LargeFolder 50
```

---

## 📚 相关文档

- **MZ_ZIP_HELPER_README.md** - Helper 函数基本用法
- **QUICK_START.md** - 快速开始指南
- **WINDOWS_GUIDE.md** - Windows 编译和使用指南

---

**最后更新**: 2025-11-26
**版本**: 1.0.0
