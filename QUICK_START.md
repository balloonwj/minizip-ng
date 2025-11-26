# Minizip-ng Helper Functions - 快速开始

## 📁 项目文件概览

### 核心封装文件
- **mz_zip_helper.h** - 头文件，定义所有 API 函数
- **mz_zip_helper.c** - 实现文件，封装了压缩/解压功能

### 示例和测试程序
- **mz_zip_helper_example.c** - 简单示例程序（命令行工具）
- **mz_zip_helper_test.c** - 完整的测试套件（支持绝对路径）

### Windows 支持
- **generate_vs2022.bat** - Windows批处理脚本（一键生成VS2022项目）
- **generate_vs2022.ps1** - PowerShell脚本（功能更强）
- **WINDOWS_GUIDE.md** - Windows 完整使用指南（中文）

### 文档
- **MZ_ZIP_HELPER_README.md** - API 详细文档和编译说明
- **QUICK_START.md** - 本文件（快速开始指南）

---

## 🚀 Windows 快速开始

### 第一步：生成 Visual Studio 2022 项目

**选项 A: 使用批处理脚本（推荐）**
```
双击运行: generate_vs2022.bat
```

**选项 B: 使用 PowerShell 脚本**
```powershell
右键 → "在终端中打开" → 运行:
.\generate_vs2022.ps1
```

### 第二步：编译项目

1. 打开生成的解决方案: `build_vs2022\minizip-ng.sln`
2. 选择 "Release" 或 "Debug" 配置
3. 按 `Ctrl+Shift+B` 编译

### 第三步：运行测试

编译后在 `build_vs2022\Release\` 目录下运行：

```batch
REM 使用默认测试目录
mz_zip_helper_test.exe

REM 使用自定义的 Windows 绝对路径
mz_zip_helper_test.exe "C:\Users\YourName\Documents\ZipTests"

REM 使用网络路径
mz_zip_helper_test.exe "\\SERVER\Share\TestFolder"
```

---

## 🐧 Linux 快速开始

### 编译

```bash
mkdir build && cd build
cmake -D MZ_BUILD_TESTS=ON ..
cmake --build . -j4
```

### 运行测试

```bash
# 使用默认测试目录
./mz_zip_helper_test

# 使用绝对路径
./mz_zip_helper_test /tmp/my_zip_tests

# 使用相对路径
./mz_zip_helper_test ../test_output
```

---

## 📝 简单示例

### C++ 代码示例

```cpp
#include "mz_zip_helper.h"
#include <stdio.h>

int main() {
    int32_t err;

    // 示例 1: 压缩单个文件
    err = mz_helper_compress_file(
        "C:\\output.zip",           // 输出 ZIP 文件
        "C:\\input.txt",            // 要压缩的文件
        NULL,                       // 无密码
        MZ_HELPER_COMPRESS_LEVEL_DEFAULT  // 默认压缩级别
    );
    if (err == MZ_OK) {
        printf("压缩成功!\n");
    }

    // 示例 2: 压缩整个文件夹
    err = mz_helper_compress_directory(
        "C:\\backup.zip",           // 输出 ZIP 文件
        "C:\\MyDocuments",          // 要压缩的文件夹
        1,                          // 包含路径
        "mypassword",               // 密码保护
        MZ_HELPER_COMPRESS_LEVEL_BEST     // 最佳压缩
    );
    if (err == MZ_OK) {
        printf("文件夹压缩成功!\n");
    }

    // 示例 3: 解压文件
    err = mz_helper_extract(
        "C:\\archive.zip",          // ZIP 文件
        "C:\\extracted",            // 解压目标目录
        "mypassword",               // 密码（如果有）
        1                           // 覆盖已存在的文件
    );
    if (err == MZ_OK) {
        printf("解压成功!\n");
    }

    return 0;
}
```

---

## 🔧 命令行工具使用

### mz_zip_helper_example.exe

```batch
REM 压缩文件
mz_zip_helper_example.exe -c output.zip myfile.txt

REM 压缩文件（带密码）
mz_zip_helper_example.exe -c secure.zip myfile.txt mypassword

REM 压缩文件夹
mz_zip_helper_example.exe -cd backup.zip "C:\MyFolder"

REM 解压文件
mz_zip_helper_example.exe -x archive.zip "C:\Output"

REM 解压文件（带密码）
mz_zip_helper_example.exe -x secure.zip "C:\Output" mypassword

REM 列出 ZIP 文件内容
mz_zip_helper_example.exe -l archive.zip
```

### mz_zip_helper_test.exe

完整的测试套件，包含12个测试用例：

```batch
REM 默认测试（在当前目录创建 mz_test_dir）
mz_zip_helper_test.exe

REM 指定测试目录（支持绝对路径）
mz_zip_helper_test.exe "C:\Temp\ZipTests"

REM 网络路径测试
mz_zip_helper_test.exe "\\NetworkShare\TestFolder"
```

**测试内容包括：**
- ✅ 单文件压缩（绝对路径）
- ✅ 密码保护压缩
- ✅ 目录压缩（递归）
- ✅ 多文件/多文件夹压缩
- ✅ 不同压缩级别（STORE, FAST, BEST）
- ✅ 列出 ZIP 内容
- ✅ 解压到绝对路径
- ✅ 密码保护解压
- ✅ 单文件解压

---

## 💡 常用函数速查

| 函数 | 功能 | 用途 |
|------|------|------|
| `mz_helper_compress_file()` | 压缩单个文件 | 快速压缩一个文件 |
| `mz_helper_compress_directory()` | 压缩文件夹 | 递归压缩整个目录 |
| `mz_helper_compress_paths()` | 压缩多个路径 | 压缩多个文件和文件夹 |
| `mz_helper_extract()` | 解压 ZIP 文件 | 解压所有内容到目录 |
| `mz_helper_extract_file()` | 解压单个文件 | 从 ZIP 中提取特定文件 |
| `mz_helper_list_files()` | 列出 ZIP 内容 | 查看 ZIP 文件包含的文件 |

---

## 🎯 压缩级别说明

| 常量 | 值 | 说明 | 速度 | 压缩率 |
|------|---|------|------|--------|
| `MZ_HELPER_COMPRESS_LEVEL_STORE` | 0 | 仅存储，不压缩 | 最快 | 无 |
| `MZ_HELPER_COMPRESS_LEVEL_FAST` | 1 | 快速压缩 | 快 | 低 |
| `MZ_HELPER_COMPRESS_LEVEL_DEFAULT` | -1 | 默认（平衡） | 中等 | 中等 |
| `MZ_HELPER_COMPRESS_LEVEL_BEST` | 9 | 最佳压缩 | 慢 | 高 |

---

## ❓ 常见问题速查

### Q: 如何处理中文路径？
**A:** 确保使用 UTF-8 编码，在 VS 中添加编译选项 `/utf-8`

### Q: 支持的最大文件大小？
**A:** 支持 ZIP64 格式，可处理 > 4GB 的文件

### Q: 如何在自己的项目中使用？
**A:**
1. 包含头文件: `#include "mz_zip_helper.h"`
2. 链接库文件: `minizip.lib`
3. 参考 `WINDOWS_GUIDE.md` 详细配置步骤

### Q: 编译错误怎么办？
**A:** 查看 `WINDOWS_GUIDE.md` 的"常见问题"章节

---

## 📚 详细文档

- **API 详细说明**: 查看 `MZ_ZIP_HELPER_README.md`
- **Windows 完整指南**: 查看 `WINDOWS_GUIDE.md`
- **原项目文档**: 查看 `README.md`

---

## 🔗 相关资源

- **项目主页**: https://github.com/zlib-ng/minizip-ng
- **CMake 下载**: https://cmake.org/download/
- **Visual Studio 下载**: https://visualstudio.microsoft.com/

---

## ✅ 测试状态

| 平台 | 状态 | 说明 |
|------|------|------|
| Linux | ✅ 已测试 | 所有12个测试通过 |
| Windows VS2022 | ⚙️ 待测试 | CMake 配置已就绪 |
| Windows VS2019 | ⚙️ 待测试 | 理论兼容 |

---

**Last Updated**: 2025-11-26
**License**: zlib License
