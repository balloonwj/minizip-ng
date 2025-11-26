# Minizip-ng 简易封装函数使用指南

本文档介绍了为 minizip-ng 库创建的简易封装函数，用于简化文件和文件夹的 Zip 压缩和解压操作。

## 概述

封装函数提供了更简单的 API 来执行常见的 Zip 操作：
- 压缩单个文件
- 压缩整个文件夹（递归）
- 压缩多个文件和文件夹
- 解压 Zip 文件到指定目录
- 解压 Zip 文件中的单个文件
- 列出 Zip 文件中的所有文件

## 文件说明

- `mz_zip_helper.h` - 头文件，包含所有 API 函数声明
- `mz_zip_helper.c` - 实现文件
- `mz_zip_helper_example.c` - 示例程序，演示如何使用封装函数

## API 函数

### 1. 压缩单个文件

```c
int32_t mz_helper_compress_file(const char *zip_path, const char *file_path,
                                const char *password, int16_t compress_level);
```

**参数：**
- `zip_path` - 输出 Zip 文件路径
- `file_path` - 要压缩的文件路径
- `password` - 加密密码（NULL 表示不加密）
- `compress_level` - 压缩级别（0-9，或 `MZ_HELPER_COMPRESS_LEVEL_DEFAULT`）

**示例：**
```c
// 压缩文件，使用默认压缩级别，无密码
mz_helper_compress_file("output.zip", "myfile.txt", NULL, MZ_HELPER_COMPRESS_LEVEL_DEFAULT);

// 压缩文件，使用最高压缩级别，带密码
mz_helper_compress_file("output.zip", "myfile.txt", "mypassword", MZ_HELPER_COMPRESS_LEVEL_BEST);
```

### 2. 压缩目录（递归）

```c
int32_t mz_helper_compress_directory(const char *zip_path, const char *dir_path,
                                     uint8_t include_path, const char *password,
                                     int16_t compress_level);
```

**参数：**
- `zip_path` - 输出 Zip 文件路径
- `dir_path` - 要压缩的目录路径
- `include_path` - 1 表示在压缩包中包含目录路径，0 表示只压缩内容
- `password` - 加密密码（NULL 表示不加密）
- `compress_level` - 压缩级别（0-9）

**示例：**
```c
// 压缩整个目录，包含目录路径
mz_helper_compress_directory("backup.zip", "MyDocuments", 1, NULL, MZ_HELPER_COMPRESS_LEVEL_DEFAULT);

// 压缩目录内容，不包含目录路径本身
mz_helper_compress_directory("backup.zip", "MyDocuments", 0, "password", MZ_HELPER_COMPRESS_LEVEL_FAST);
```

### 3. 压缩多个文件/文件夹

```c
int32_t mz_helper_compress_paths(const char *zip_path, const char **paths,
                                 int32_t path_count, uint8_t include_path,
                                 const char *password, int16_t compress_level);
```

**参数：**
- `zip_path` - 输出 Zip 文件路径
- `paths` - 文件/目录路径数组
- `path_count` - 路径数量
- `include_path` - 1 表示包含路径，0 表示只包含文件名
- `password` - 加密密码
- `compress_level` - 压缩级别

**示例：**
```c
const char *paths[] = {"file1.txt", "file2.txt", "MyFolder"};
mz_helper_compress_paths("archive.zip", paths, 3, 1, NULL, MZ_HELPER_COMPRESS_LEVEL_DEFAULT);
```

### 4. 解压 Zip 文件

```c
int32_t mz_helper_extract(const char *zip_path, const char *dest_dir,
                          const char *password, uint8_t overwrite);
```

**参数：**
- `zip_path` - Zip 文件路径
- `dest_dir` - 目标目录（NULL 表示当前目录）
- `password` - 解密密码（NULL 表示无密码）
- `overwrite` - 1 表示覆盖已存在的文件，0 表示跳过

**示例：**
```c
// 解压到指定目录，覆盖已存在的文件
mz_helper_extract("archive.zip", "./extracted", NULL, 1);

// 解压到当前目录，不覆盖已存在的文件
mz_helper_extract("archive.zip", NULL, "password", 0);
```

### 5. 解压单个文件

```c
int32_t mz_helper_extract_file(const char *zip_path, const char *filename,
                               const char *dest_dir, const char *password,
                               uint8_t overwrite);
```

**参数：**
- `zip_path` - Zip 文件路径
- `filename` - 要解压的文件名（在压缩包中的名称）
- `dest_dir` - 目标目录
- `password` - 解密密码
- `overwrite` - 是否覆盖

**示例：**
```c
mz_helper_extract_file("archive.zip", "myfile.txt", "./extracted", NULL, 1);
```

### 6. 列出 Zip 文件内容

```c
typedef int32_t (*mz_helper_list_cb)(void *userdata, const char *filename,
                                     int64_t uncompressed_size, int64_t compressed_size);

int32_t mz_helper_list_files(const char *zip_path, mz_helper_list_cb callback,
                             void *userdata);
```

**示例：**
```c
int32_t list_callback(void *userdata, const char *filename,
                      int64_t uncompressed_size, int64_t compressed_size) {
    printf("%s (%" PRId64 " bytes)\n", filename, uncompressed_size);
    return MZ_OK;
}

mz_helper_list_files("archive.zip", list_callback, NULL);
```

## 在 Windows Visual Studio 中编译

### 方法 1: 使用 CMake GUI

1. 下载并安装 [CMake](https://cmake.org/download/)（3.13 或更高版本）

2. 打开 CMake GUI：
   - **Where is the source code**: 选择 minizip-ng 源代码目录
   - **Where to build the binaries**: 选择一个构建目录（例如 `build`）

3. 点击 **Configure** 按钮：
   - 选择你的 Visual Studio 版本（例如 "Visual Studio 17 2022"）
   - 选择平台（x64 或 Win32）
   - 点击 **Finish**

4. 配置选项：
   - 勾选 `MZ_BUILD_TESTS` 以编译示例程序
   - 根据需要配置其他选项（压缩算法、加密等）
   - 点击 **Configure** 再次配置
   - 点击 **Generate** 生成 Visual Studio 解决方案

5. 打开生成的解决方案：
   - 在构建目录中找到 `minizip-ng.sln`
   - 双击打开 Visual Studio
   - 选择 Release 或 Debug 配置
   - 右键点击解决方案，选择 **生成解决方案**

### 方法 2: 使用 CMake 命令行

打开 **Developer Command Prompt for VS**（或 **x64 Native Tools Command Prompt**）：

```batch
# 创建构建目录
mkdir build
cd build

# 生成 Visual Studio 解决方案（VS 2022，64位）
cmake -G "Visual Studio 17 2022" -A x64 -D MZ_BUILD_TESTS=ON ..

# 编译（Release 配置）
cmake --build . --config Release

# 或者在 Visual Studio 中打开解决方案
start minizip-ng.sln
```

对于其他 Visual Studio 版本：
- Visual Studio 2019: `cmake -G "Visual Studio 16 2019" -A x64 ..`
- Visual Studio 2017: `cmake -G "Visual Studio 15 2017" -A x64 ..`

### 编译输出

编译成功后，在 `build/Release` 或 `build/Debug` 目录中会生成：
- `minizip.lib` (或 `minizip.dll`) - minizip-ng 库
- `mz_zip_helper_example.exe` - 示例程序

## 使用示例程序

编译完成后，可以使用 `mz_zip_helper_example.exe` 来测试封装函数：

```batch
# 压缩单个文件
mz_zip_helper_example.exe -c output.zip myfile.txt

# 压缩单个文件（带密码）
mz_zip_helper_example.exe -c output.zip myfile.txt mypassword

# 压缩目录
mz_zip_helper_example.exe -cd backup.zip MyDocuments

# 解压文件
mz_zip_helper_example.exe -x archive.zip ./extracted

# 解压文件（带密码）
mz_zip_helper_example.exe -x archive.zip ./extracted mypassword

# 列出 Zip 文件内容
mz_zip_helper_example.exe -l archive.zip
```

## 在你的项目中使用

### 1. 包含头文件

```c
#include "mz_zip_helper.h"
```

### 2. 链接库

在 Visual Studio 项目中：
- 添加 minizip-ng 的 include 目录到 **项目属性 → C/C++ → 常规 → 附加包含目录**
- 添加 `minizip.lib` 到 **项目属性 → 链接器 → 输入 → 附加依赖项**
- 确保库文件路径在 **项目属性 → 链接器 → 常规 → 附加库目录** 中

### 3. 简单示例

```c
#include "mz_zip_helper.h"
#include <stdio.h>

int main() {
    int32_t err;

    // 压缩文件
    printf("压缩文件...\n");
    err = mz_helper_compress_file("test.zip", "myfile.txt", NULL, -1);
    if (err == MZ_OK) {
        printf("压缩成功!\n");
    } else {
        printf("压缩失败: %d\n", err);
        return 1;
    }

    // 解压文件
    printf("解压文件...\n");
    err = mz_helper_extract("test.zip", "./output", NULL, 1);
    if (err == MZ_OK) {
        printf("解压成功!\n");
    } else {
        printf("解压失败: %d\n", err);
        return 1;
    }

    return 0;
}
```

## 错误代码

函数返回值：
- `MZ_OK` (0) - 成功
- `MZ_PARAM_ERROR` - 参数错误
- `MZ_MEM_ERROR` - 内存分配错误
- `MZ_EXIST_ERROR` - 文件已存在
- 其他错误代码请参考 `mz.h`

## 压缩级别

预定义的压缩级别常量：
- `MZ_HELPER_COMPRESS_LEVEL_STORE` (0) - 仅存储，不压缩
- `MZ_HELPER_COMPRESS_LEVEL_FAST` (1) - 最快压缩
- `MZ_HELPER_COMPRESS_LEVEL_DEFAULT` (-1) - 默认压缩
- `MZ_HELPER_COMPRESS_LEVEL_BEST` (9) - 最佳压缩

## 注意事项

1. **路径分隔符**: 在 Windows 上可以使用 `\` 或 `/` 作为路径分隔符
2. **密码保护**: 使用密码时，确保密码字符串以 NULL 结尾
3. **文件覆盖**: 使用 `overwrite` 参数控制是否覆盖已存在的文件
4. **错误处理**: 始终检查函数返回值以处理错误情况
5. **大文件支持**: 库支持 ZIP64，可以处理大于 4GB 的文件

## 许可证

这些封装函数与 minizip-ng 库使用相同的 zlib 许可证。

## 相关链接

- [minizip-ng GitHub](https://github.com/zlib-ng/minizip-ng)
- [CMake 官网](https://cmake.org/)
- [Visual Studio 下载](https://visualstudio.microsoft.com/)
