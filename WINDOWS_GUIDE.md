# Minizip-ng Windows Visual Studio 2022 使用指南

本指南详细说明如何在 Windows 系统上使用 Visual Studio 2022 编译和使用 minizip-ng 的封装函数。

## 目录

1. [系统要求](#系统要求)
2. [快速开始](#快速开始)
3. [生成 Visual Studio 2022 项目](#生成-visual-studio-2022-项目)
4. [在 Visual Studio 中编译](#在-visual-studio-中编译)
5. [使用绝对路径测试](#使用绝对路径测试)
6. [示例：Windows 绝对路径](#示例windows-绝对路径)
7. [常见问题](#常见问题)

---

## 系统要求

- **操作系统**: Windows 10/11 (64-bit)
- **IDE**: Visual Studio 2022 (Community/Professional/Enterprise)
  - 下载地址: https://visualstudio.microsoft.com/
  - 必须安装 "使用 C++ 的桌面开发" 工作负载
- **CMake**: 3.13 或更高版本
  - 下载地址: https://cmake.org/download/
  - 建议安装时选择 "Add CMake to the system PATH"
- **可选依赖库**:
  - ZLIB (会自动下载)
  - BZIP2 (会自动下载)
  - LZMA (会自动下载)

---

## 快速开始

### 方法 1: 使用批处理脚本（推荐）

1. 打开 `minizip-ng` 目录
2. 双击运行 `generate_vs2022.bat`
3. 等待 CMake 生成项目
4. 选择 "Y" 自动打开 Visual Studio 2022

### 方法 2: 使用 PowerShell 脚本

1. 在 `minizip-ng` 目录中右键点击，选择 "在终端中打开" 或 "Open PowerShell window here"
2. 运行以下命令：
   ```powershell
   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
   .\generate_vs2022.ps1
   ```
3. 按提示操作

---

## 生成 Visual Studio 2022 项目

### 使用批处理脚本

`generate_vs2022.bat` 会自动完成以下操作：

1. 检测 CMake 是否安装
2. 创建 `build_vs2022` 目录
3. 生成 Visual Studio 2022 解决方案（64位）
4. 配置所有测试程序

**脚本会启用以下功能**:
- ✅ ZLIB 压缩
- ✅ BZIP2 压缩
- ✅ LZMA/XZ 压缩
- ✅ PKWARE 传统加密
- ✅ WinZIP AES 加密
- ✅ 兼容层
- ✅ 测试程序（包括绝对路径测试）

### 手动生成（命令行方式）

如果你想手动控制生成过程：

#### 1. 打开 Developer Command Prompt for VS 2022

从开始菜单找到：
```
Visual Studio 2022 → Developer Command Prompt for VS 2022
```

#### 2. 导航到项目目录

```batch
cd C:\path\to\minizip-ng
```

#### 3. 创建并进入构建目录

```batch
mkdir build_vs2022
cd build_vs2022
```

#### 4. 运行 CMake

```batch
cmake -G "Visual Studio 17 2022" -A x64 -D MZ_BUILD_TESTS=ON ..
```

#### 5. 生成完成

你会看到类似以下输出：
```
-- Configuring done
-- Generating done
-- Build files have been written to: C:/path/to/minizip-ng/build_vs2022
```

---

## 在 Visual Studio 中编译

### 打开解决方案

1. 双击 `build_vs2022\minizip-ng.sln` 打开 Visual Studio 2022
2. 或者在 Visual Studio 中: **文件 → 打开 → 项目/解决方案** → 选择 `minizip-ng.sln`

### 选择配置

在工具栏上选择构建配置：
- **Debug**: 用于调试（包含符号信息，未优化）
- **Release**: 用于发布（优化版本）

### 编译整个解决方案

1. 在菜单栏选择: **生成 → 生成解决方案** (或按 `Ctrl+Shift+B`)
2. 等待编译完成（可能需要几分钟）

### 编译后的文件位置

编译成功后，可执行文件会在以下目录：

**Release 版本**:
```
build_vs2022\Release\
  ├── minizip.lib                  (静态库)
  ├── minizip.exe                  (主程序)
  ├── mz_zip_helper_example.exe    (简单示例)
  └── mz_zip_helper_test.exe       (测试套件)
```

**Debug 版本**:
```
build_vs2022\Debug\
  ├── minizip.lib
  ├── minizip.exe
  ├── mz_zip_helper_example.exe
  └── mz_zip_helper_test.exe
```

### 设置启动项目

要运行特定程序：
1. 在 **解决方案资源管理器** 中右键点击项目（如 `mz_zip_helper_test`）
2. 选择 **设为启动项目**
3. 按 `F5` 运行（调试模式）或 `Ctrl+F5` 运行（不调试）

---

## 使用绝对路径测试

`mz_zip_helper_test.exe` 是一个全面的测试程序，专门设计用于测试 Windows 绝对路径的支持。

### 运行测试

#### 方法 1: 使用默认测试目录

在 Visual Studio 中按 `Ctrl+F5` 运行，或在命令行中：

```batch
cd build_vs2022\Release
mz_zip_helper_test.exe
```

这会在当前目录创建 `mz_test_dir` 并运行所有测试。

#### 方法 2: 指定自定义测试目录（绝对路径）

```batch
mz_zip_helper_test.exe "C:\Users\YourName\Documents\ZipTests"
```

#### 方法 3: 使用 UNC 路径（网络路径）

```batch
mz_zip_helper_test.exe "\\NetworkShare\TestFolder"
```

### 测试内容

测试程序会执行以下操作：

1. **创建测试文件结构**
   ```
   测试目录\
     ├── test1.txt
     ├── test2.txt
     └── subdir\
         └── test3.txt
   ```

2. **压缩测试**
   - 压缩单个文件（绝对路径）
   - 压缩带密码的文件
   - 压缩整个目录
   - 压缩多个文件和文件夹
   - 测试不同压缩级别 (STORE, FAST, BEST)

3. **解压测试**
   - 解压到绝对路径
   - 解压带密码的压缩包
   - 解压单个文件

4. **列表测试**
   - 列出压缩包中的所有文件

### 测试输出示例

```
========================================
Minizip-ng Helper Functions Test Suite
========================================

Current working directory: C:\Users\YourName\Documents\Projects\minizip-ng
Test directory: C:\Users\YourName\Documents\ZipTests

Creating test directory structure...
Test structure created successfully.

Running tests:
----------------------------------------
[PASS] Compress single file (absolute path)
[PASS] Compress single file with password
[PASS] Compress directory (absolute path, no path included)
[PASS] Compress directory (absolute path, path included)
[PASS] Compress multiple paths
[PASS] Compress with STORE level (no compression)
[PASS] Compress with FAST level
[PASS] Compress with BEST level
  Listing files in test_directory.zip:
    [0] test1.txt (45 bytes -> 47 bytes)
    [1] test2.txt (45 bytes -> 47 bytes)
    [2] subdir/ (0 bytes -> 0 bytes)
    [3] subdir/test3.txt (48 bytes -> 50 bytes)
[PASS] List files in archive
[PASS] Extract archive (absolute path)
[PASS] Extract archive with password
[PASS] Extract single file
----------------------------------------

Test Summary:
  Total:  11
  Passed: 11
  Failed: 0

RESULT: All tests PASSED!
```

---

## 示例：Windows 绝对路径

### 示例 1: 压缩文件到 Documents 文件夹

```batch
mz_zip_helper_example.exe -c "C:\Users\YourName\Documents\backup.zip" "C:\Users\YourName\Desktop\important.docx"
```

### 示例 2: 压缩整个文件夹

```batch
mz_zip_helper_example.exe -cd "C:\Backup\photos_2024.zip" "C:\Users\YourName\Pictures\2024"
```

### 示例 3: 压缩到网络共享位置

```batch
mz_zip_helper_example.exe -cd "\\SERVER\Backup\myfiles.zip" "C:\Users\YourName\Documents"
```

### 示例 4: 解压到指定位置

```batch
mz_zip_helper_example.exe -x "C:\Downloads\archive.zip" "C:\Temp\extracted"
```

### 示例 5: 带密码压缩

```batch
mz_zip_helper_example.exe -c "C:\Secure\confidential.zip" "C:\Documents\secret.txt" MyPassword123
```

### 示例 6: 带密码解压

```batch
mz_zip_helper_example.exe -x "C:\Secure\confidential.zip" "C:\Temp" MyPassword123
```

---

## 在你的 C++ 项目中使用

### 步骤 1: 添加包含目录

在 Visual Studio 项目中：

1. 右键点击项目 → **属性**
2. **C/C++** → **常规** → **附加包含目录**
3. 添加 minizip-ng 的路径：
   ```
   C:\path\to\minizip-ng
   ```

### 步骤 2: 添加库目录

1. **链接器** → **常规** → **附加库目录**
2. 添加：
   ```
   C:\path\to\minizip-ng\build_vs2022\Release
   ```

### 步骤 3: 添加库依赖

1. **链接器** → **输入** → **附加依赖项**
2. 添加：
   ```
   minizip.lib
   ```

### 步骤 4: 在代码中使用

```cpp
#include "mz_zip_helper.h"
#include <stdio.h>

int main() {
    // 使用 Windows 绝对路径压缩文件
    int32_t err = mz_helper_compress_file(
        "C:\\Users\\YourName\\Documents\\output.zip",
        "C:\\Users\\YourName\\Desktop\\myfile.txt",
        NULL,  // 无密码
        -1     // 默认压缩级别
    );

    if (err == MZ_OK) {
        printf("压缩成功!\n");
    } else {
        printf("压缩失败，错误代码: %d\n", err);
        return 1;
    }

    // 解压到指定目录
    err = mz_helper_extract(
        "C:\\Users\\YourName\\Documents\\output.zip",
        "C:\\Users\\YourName\\Desktop\\extracted",
        NULL,  // 无密码
        1      // 覆盖已存在的文件
    );

    if (err == MZ_OK) {
        printf("解压成功!\n");
    } else {
        printf("解压失败，错误代码: %d\n", err);
        return 1;
    }

    return 0;
}
```

---

## 常见问题

### Q1: CMake 提示 "not found"

**答**: 确保 CMake 已添加到系统 PATH。重新安装 CMake 时选择 "Add CMake to the system PATH for all users"。

### Q2: 编译时出现 "无法打开包含文件" 错误

**答**: 检查是否正确设置了包含目录。确保路径指向 minizip-ng 的根目录。

### Q3: 链接时出现 "无法解析的外部符号" 错误

**答**:
1. 确保链接了正确的 `minizip.lib`
2. 确认库目录路径正确
3. 确认使用的配置（Debug/Release）与库的配置匹配

### Q4: 运行时出现 "找不到 DLL" 错误

**答**: minizip-ng 默认编译为静态库，不应该有 DLL 依赖。如果遇到此问题，检查是否启用了动态链接选项。

### Q5: 如何处理中文路径？

**答**: Windows 上的路径应使用 UTF-8 编码。确保：
1. 源文件保存为 UTF-8 编码
2. 在 Visual Studio 中设置: **C/C++** → **命令行** → 添加 `/utf-8`

示例：
```cpp
// 正确处理中文路径
mz_helper_compress_file(
    "C:\\用户\\文档\\压缩文件.zip",  // UTF-8 编码
    "C:\\用户\\桌面\\测试.txt",
    NULL, -1
);
```

### Q6: 能否压缩到系统保护的目录（如 C:\Windows）？

**答**: 需要管理员权限。以管理员身份运行 Visual Studio 或你的程序：
1. 右键点击 Visual Studio 2022
2. 选择 "以管理员身份运行"

### Q7: 如何在压缩时显示进度？

**答**: 当前的简化 API 不支持进度回调。如需进度显示，请使用底层的 `mz_zip_writer` API 并设置进度回调。

### Q8: 支持哪些压缩算法？

**答**:
- DEFLATE (默认，最常用)
- BZIP2 (更好的压缩率)
- LZMA (最佳压缩率)
- STORE (不压缩)

默认使用 DEFLATE 算法，这是最兼容的选择。

### Q9: 最大能压缩多大的文件？

**答**: 支持 ZIP64 格式，理论上可以处理超过 4GB 的文件和压缩包。

### Q10: 能否在没有 Visual Studio 的电脑上运行编译的程序？

**答**: Release 版本通常可以独立运行。如果缺少运行时库，需要安装 Visual C++ Redistributable:
- 下载地址: https://aka.ms/vs/17/release/vc_redist.x64.exe

---

## 技术支持

如有问题，请访问：
- GitHub Issues: https://github.com/zlib-ng/minizip-ng/issues
- 官方文档: https://github.com/zlib-ng/minizip-ng

---

## 许可证

本项目使用 zlib 许可证。详见 LICENSE 文件。
