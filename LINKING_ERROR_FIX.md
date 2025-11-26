# 快速修复指南

## 🔴 问题原因

你遇到的链接错误是因为：
- BCrypt* 错误 → 缺少 bcrypt.lib（Windows 加密库）
- BZ2_* 错误 → 缺少 bzip2.lib
- lzma_* 错误 → 缺少 liblzma.lib
- deflate/inflate 错误 → 缺少 zlib.lib

这些都是完整版需要的依赖库。

---

## ✅ 解决方案 1：使用精简版（强烈推荐）

### 步骤：

1. **关闭 Visual Studio**

2. **删除旧的构建目录**
   ```batch
   cd C:\minizip-ng
   rmdir /S /Q build_vs2022
   ```

3. **运行精简版生成脚本**
   ```batch
   generate_vs2022_minimal.bat
   ```

4. **打开新生成的解决方案**
   ```batch
   cd build_vs2022_minimal
   start minizip-ng.sln
   ```

5. **编译（Ctrl+Shift+B）**

### 为什么精简版能解决问题？

精简版只需要 **ZLIB**（会自动下载），不需要：
- ❌ BZIP2
- ❌ LZMA
- ❌ ZSTD
- ❌ BCrypt（加密）

编译快、问题少、够用！

---

## ✅ 解决方案 2：修复完整版（如果你坚持要用完整版）

### 选项 A：让 CMake 自动下载依赖

1. **关闭 Visual Studio**

2. **删除构建目录并重新生成**
   ```batch
   cd C:\minizip-ng
   rmdir /S /Q build_vs2022
   mkdir build_vs2022
   cd build_vs2022
   ```

3. **生成时启用自动下载**
   ```batch
   cmake -G "Visual Studio 17 2022" -A x64 ^
         -D MZ_BUILD_TESTS=ON ^
         -D MZ_FETCH_LIBS=ON ^
         -D MZ_FORCE_FETCH_LIBS=ON ^
         ..
   ```

   **注意**：这会从网络下载依赖库，可能需要几分钟。

4. **编译**
   ```batch
   cmake --build . --config Release
   ```

### 选项 B：手动添加库依赖

如果你已经生成了项目，可以在 Visual Studio 中手动修复：

**对于 minizip 项目：**

1. 右键 `minizip` 项目 → 属性
2. **链接器 → 输入 → 附加依赖项**
3. 添加以下库：
   ```
   bcrypt.lib
   zlib.lib
   bzip2.lib
   liblzma.lib
   ```

**对于 mz_zip_helper_test 项目：**

1. 右键 `mz_zip_helper_test` 项目 → 属性
2. **链接器 → 输入 → 附加依赖项**
3. 确保包含：
   ```
   minizip.lib
   bcrypt.lib
   zlib.lib
   bzip2.lib
   liblzma.lib
   ```

但这个方法**不推荐**，因为你还需要提供这些库的路径。

---

## ✅ 解决方案 3：完全禁用不需要的功能

如果你不想用精简版脚本，可以手动禁用这些功能：

```batch
rmdir /S /Q build_vs2022
mkdir build_vs2022
cd build_vs2022

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

这实际上就是精简版的配置。

---

## 🎯 我的建议

**按这个顺序尝试：**

1. ⭐ **首选：使用精简版**
   ```batch
   generate_vs2022_minimal.bat
   ```
   - 快速（3分钟编译）
   - 简单（只需要 ZLIB）
   - 够用（99% 的场景）

2. 如果需要加密功能，再用完整版：
   ```batch
   generate_vs2022.bat
   ```
   需要确保网络畅通，让 CMake 自动下载依赖。

---

## 🔍 如何判断哪个脚本生成的？

检查构建目录名：
- `build_vs2022` → 完整版（有依赖问题）
- `build_vs2022_minimal` → 精简版（推荐）

---

## 📝 快速操作步骤（推荐）

```batch
REM 1. 移动到短路径（如果还没有）
cd C:\minizip-ng

REM 2. 清理旧构建
rmdir /S /Q build_vs2022

REM 3. 使用精简版生成
generate_vs2022_minimal.bat

REM 4. 编译
cd build_vs2022_minimal
cmake --build . --config Release

REM 5. 测试
cd Release
mz_zip_helper_test.exe C:\Temp\ZipTests
```

这样应该就能解决问题了！
