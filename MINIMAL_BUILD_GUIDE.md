# Minizip-ng 精简版构建指南

本指南说明如何构建一个**精简版本**的 minizip-ng，只包含基本的 ZIP 压缩和解压功能。

## 📦 版本对比

### 完整版 (generate_vs2022.bat)

**包含功能：**
- ✅ ZLIB (DEFLATE) - 标准 ZIP 压缩
- ✅ BZIP2 - 更高压缩率
- ✅ LZMA/XZ - 最高压缩率
- ✅ ZSTD - 现代高性能压缩
- ✅ PKCRYPT - 传统加密
- ✅ WinZIP AES - 现代加密

**优点：**
- 功能完整，支持所有 ZIP 特性
- 可处理各种加密的 ZIP 文件
- 支持多种压缩算法

**缺点：**
- 编译时间较长
- 需要下载多个第三方库
- 最终程序体积较大
- 依赖项较多

---

### 精简版 (generate_vs2022_minimal.bat) ⭐ 推荐

**包含功能：**
- ✅ ZLIB (DEFLATE) - 标准 ZIP 压缩
- ✅ STORE - 无压缩存储

**排除功能：**
- ❌ BZIP2
- ❌ LZMA/XZ
- ❌ ZSTD
- ❌ PKCRYPT（传统加密）
- ❌ WinZIP AES（加密）

**优点：**
- ✅ 编译速度快（2-3分钟 vs 10+分钟）
- ✅ 最终程序体积小（约 200KB vs 2MB+）
- ✅ 依赖少，只需要 ZLIB（会自动下载）
- ✅ 覆盖 99% 的常见使用场景
- ✅ 兼容所有标准 ZIP 文件

**缺点：**
- ❌ 不能解压带密码的 ZIP 文件
- ❌ 不能创建加密的 ZIP 文件
- ❌ 不支持 BZIP2/LZMA/ZSTD 压缩的文件

**适用场景：**
- 基本的文件打包和解包
- 不需要密码保护
- 只需要标准 DEFLATE 压缩（最常用）
- 追求编译速度和程序体积

---

## 🚀 使用精简版

### Windows 用户

#### 方法 1：批处理脚本（推荐）

```batch
# 双击运行
generate_vs2022_minimal.bat
```

#### 方法 2：PowerShell 脚本

```powershell
# 右键 → "在终端中打开"
.\generate_vs2022_minimal.ps1
```

#### 方法 3：手动命令行

```batch
# 打开 Developer Command Prompt for VS 2022
mkdir build_vs2022_minimal
cd build_vs2022_minimal

cmake -G "Visual Studio 17 2022" -A x64 ^
      -D MZ_BUILD_TESTS=ON ^
      -D MZ_ZLIB=ON ^
      -D MZ_BZIP2=OFF ^
      -D MZ_LZMA=OFF ^
      -D MZ_ZSTD=OFF ^
      -D MZ_PKCRYPT=OFF ^
      -D MZ_WZAES=OFF ^
      -D MZ_COMPAT=ON ^
      ..

cmake --build . --config Release
```

---

## 📊 功能对比表

| 功能 | 完整版 | 精简版 | 说明 |
|------|:------:|:------:|------|
| **压缩算法** |
| DEFLATE (标准 ZIP) | ✅ | ✅ | 最常用，兼容性最好 |
| STORE (无压缩) | ✅ | ✅ | 直接存储，速度最快 |
| BZIP2 | ✅ | ❌ | 更高压缩率，速度慢 |
| LZMA/XZ | ✅ | ❌ | 最高压缩率，速度很慢 |
| ZSTD | ✅ | ❌ | 现代算法，平衡速度和压缩率 |
| **加密功能** |
| PKWARE 传统加密 | ✅ | ❌ | 旧式加密，不安全 |
| WinZIP AES | ✅ | ❌ | 现代加密，安全 |
| **其他特性** |
| 兼容层 | ✅ | ✅ | 支持旧版 API |
| ZIP64 (大文件) | ✅ | ✅ | 支持 >4GB 文件 |
| UTF-8 文件名 | ✅ | ✅ | 中文文件名支持 |
| 目录递归 | ✅ | ✅ | 压缩整个文件夹 |
| **构建特性** |
| 编译时间 | ~10分钟 | ~3分钟 | 估计值 |
| 程序体积 | ~2MB | ~200KB | Release 版本 |
| 依赖库数量 | 5+ | 1 | 仅 ZLIB |

---

## 💡 选择建议

### 使用精简版，如果你：

✅ 只需要基本的压缩/解压功能
✅ 不需要密码保护
✅ 追求快速编译
✅ 想要小体积的程序
✅ 只处理标准 ZIP 文件
✅ 首次尝试，想快速看到效果

### 使用完整版，如果你：

✅ 需要处理加密的 ZIP 文件
✅ 需要创建密码保护的压缩包
✅ 需要最高的压缩率（LZMA）
✅ 需要解压各种格式的 ZIP 文件
✅ 对编译时间不敏感

---

## 🔍 常见问题

### Q1: 精简版能解压普通的 ZIP 文件吗？

**A:** 可以！大部分 ZIP 文件都使用 DEFLATE 压缩（精简版支持）。只有少数特殊 ZIP 文件使用其他算法。

### Q2: 精简版能压缩文件吗？

**A:** 可以！使用标准的 DEFLATE 压缩算法（兼容性最好）。

### Q3: 如果遇到加密的 ZIP 文件怎么办？

**A:** 精简版无法解压加密文件。你需要：
- 使用完整版编译
- 或使用其他工具先解密

### Q4: 精简版创建的 ZIP 文件兼容性如何？

**A:** 完美兼容！DEFLATE 是 ZIP 格式的标准压缩方法，所有 ZIP 工具都支持。

### Q5: 可以同时安装精简版和完整版吗？

**A:** 可以！它们使用不同的构建目录：
- 完整版：`build_vs2022`
- 精简版：`build_vs2022_minimal`

### Q6: Helper 函数在精简版中正常工作吗？

**A:** 完全正常！所有封装的 helper 函数都能用：
- `mz_helper_compress_file()`
- `mz_helper_compress_directory()`
- `mz_helper_extract()`
- 等等...

唯一区别是不能使用密码参数（传 `NULL` 即可）。

### Q7: 测试程序能在精简版中运行吗？

**A:** 可以！`mz_zip_helper_test.exe` 会跳过密码相关的测试，其他测试都会通过。

---

## 📝 示例代码

### 精简版可以做什么

```c
#include "mz_zip_helper.h"

int main() {
    // ✅ 压缩文件 - 可以
    mz_helper_compress_file(
        "C:\\output.zip",
        "C:\\input.txt",
        NULL,  // 无密码（精简版不支持加密）
        MZ_HELPER_COMPRESS_LEVEL_BEST
    );

    // ✅ 压缩文件夹 - 可以
    mz_helper_compress_directory(
        "C:\\backup.zip",
        "C:\\MyDocuments",
        1,     // 包含路径
        NULL,  // 无密码
        MZ_HELPER_COMPRESS_LEVEL_DEFAULT
    );

    // ✅ 解压文件 - 可以（如果 ZIP 没有加密）
    mz_helper_extract(
        "C:\\archive.zip",
        "C:\\extracted",
        NULL,  // 无密码
        1      // 覆盖
    );

    // ✅ 列出文件 - 可以
    mz_helper_list_files("C:\\archive.zip", NULL, NULL);

    return 0;
}
```

### 精简版不能做什么

```c
// ❌ 创建加密的 ZIP（精简版不支持）
mz_helper_compress_file(
    "secure.zip",
    "secret.txt",
    "password",  // 精简版会忽略密码，不加密
    -1
);

// ❌ 解压加密的 ZIP（会失败）
mz_helper_extract(
    "encrypted.zip",  // 如果这个文件是加密的
    "output",
    "password",       // 精简版无法解密
    1
);
```

---

## 🎯 快速决策

**一句话建议：**

> 如果不确定选哪个，**先用精简版**。它能满足 99% 的需求，编译快，出问题少。
> 等真正需要密码保护或特殊压缩算法时，再切换到完整版。

---

## 📦 编译输出对比

### 精简版编译输出

```
build_vs2022_minimal/
  └── Release/
      ├── minizip.lib              (~150 KB)
      ├── minizip.exe              (~50 KB)
      ├── mz_zip_helper_example.exe (~50 KB)
      └── mz_zip_helper_test.exe   (~60 KB)
```

**总计：** ~300 KB

### 完整版编译输出

```
build_vs2022/
  └── Release/
      ├── minizip.lib              (~2 MB)
      ├── minizip.exe              (~500 KB)
      ├── mz_zip_helper_example.exe (~500 KB)
      └── mz_zip_helper_test.exe   (~600 KB)
      ├── zlib.lib
      ├── bzip2.lib
      ├── liblzma.lib
      └── ... (其他依赖库)
```

**总计：** ~5-10 MB

---

## 🔄 在版本间切换

### 从精简版切换到完整版

```batch
# 1. 运行完整版生成脚本
generate_vs2022.bat

# 2. 编译
cd build_vs2022
cmake --build . --config Release
```

### 从完整版切换到精简版

```batch
# 1. 运行精简版生成脚本
generate_vs2022_minimal.bat

# 2. 编译
cd build_vs2022_minimal
cmake --build . --config Release
```

两个版本可以共存，互不影响！

---

**推荐**：先尝试精简版 `generate_vs2022_minimal.bat`，快速、简单、满足大多数需求！

**最后更新**: 2025-11-26
