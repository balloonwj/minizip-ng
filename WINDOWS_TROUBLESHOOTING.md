# Windows 编译故障排除指南

## 🔴 常见问题 1：路径太长错误

### 错误信息
```
error MSB6003: 指定的任务可执行文件"CL.exe"未能运行。
System.IO.DirectoryNotFoundException: 未能找到路径...的一部分。
```

### 原因
Windows 默认最大路径长度为 **260 个字符**。如果项目路径太长（例如在 Downloads 文件夹中的长文件夹名），会导致编译失败。

### ✅ 解决方案（推荐顺序）

#### 方案 1：移动到短路径（最简单，强烈推荐）

**步骤：**

1. 将整个项目文件夹移动到更短的路径：
   ```
   推荐位置：
   C:\minizip-ng
   C:\dev\minizip-ng
   D:\projects\minizip-ng
   ```

2. 重新运行生成脚本：
   ```batch
   generate_vs2022.bat
   ```

**示例操作：**
```batch
# 在文件资源管理器中
# 1. 剪切当前文件夹
# 2. 导航到 C:\
# 3. 粘贴并重命名为 "minizip-ng"
# 4. 打开新位置，双击 generate_vs2022.bat
```

#### 方案 2：启用 Windows 长路径支持

**方法 A：通过 PowerShell（推荐）**

以管理员身份运行 PowerShell：
```powershell
# 1. 右键点击开始菜单 → Windows PowerShell (管理员)
# 2. 运行以下命令：
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" `
                 -Name "LongPathsEnabled" `
                 -Value 1 `
                 -PropertyType DWORD `
                 -Force

# 3. 重启计算机
```

**方法 B：通过注册表**

1. 按 `Win + R`，输入 `regedit`，回车
2. 导航到：
   ```
   HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem
   ```
3. 右键 → 新建 → DWORD (32位) 值
4. 命名为 `LongPathsEnabled`
5. 双击，设置值为 `1`
6. 重启计算机

**方法 C：通过组策略（专业版/企业版）**

1. 按 `Win + R`，输入 `gpedit.msc`，回车
2. 导航到：
   ```
   计算机配置 → 管理模板 → 系统 → 文件系统
   ```
3. 双击"启用 Win32 长路径"
4. 选择"已启用"
5. 点击"确定"
6. 重启计算机

**注意：** 启用后仍需要应用程序支持，CMake 和 VS2022 都支持长路径。

#### 方案 3：手动使用更短的构建目录名

修改生成脚本中的构建目录名：

```batch
# 编辑 generate_vs2022.bat
# 将 build_vs2022 改为 b
set BUILD_DIR=b

# 这样可以节省几个字符
```

---

## 🔴 常见问题 2：找不到 CMake

### 错误信息
```
'cmake' 不是内部或外部命令，也不是可运行的程序
```

### 解决方案

1. **安装 CMake**：
   - 下载：https://cmake.org/download/
   - 选择 Windows x64 Installer
   - **重要**：安装时勾选 "Add CMake to the system PATH for all users"

2. **验证安装**：
   ```batch
   cmake --version
   ```

3. **如果已安装但仍报错**：
   - 手动添加 CMake 到 PATH
   - 或重启命令提示符/PowerShell

---

## 🔴 常见问题 3：找不到 Visual Studio

### 错误信息
```
CMake Error: CMAKE_C_COMPILER not set
Could not find Visual Studio
```

### 解决方案

1. **确认已安装 Visual Studio 2022**
   - 下载：https://visualstudio.microsoft.com/
   - 必须安装"使用 C++ 的桌面开发"工作负载

2. **使用 Developer Command Prompt**：
   ```
   开始菜单 → Visual Studio 2022 → Developer Command Prompt for VS 2022
   ```
   然后运行：
   ```batch
   cd C:\minizip-ng
   generate_vs2022.bat
   ```

3. **检查 Visual Studio 版本**：
   - 如果是 VS2019，使用：
     ```batch
     cmake -G "Visual Studio 16 2019" -A x64 ..
     ```
   - 如果是 VS2017，使用：
     ```batch
     cmake -G "Visual Studio 15 2017 Win64" ..
     ```

---

## 🔴 常见问题 4：权限被拒绝

### 错误信息
```
拒绝访问
Access is denied
```

### 解决方案

1. **以管理员身份运行**：
   - 右键 `generate_vs2022.bat`
   - 选择"以管理员身份运行"

2. **检查文件夹权限**：
   - 右键项目文件夹 → 属性 → 安全
   - 确保当前用户有"完全控制"权限

3. **关闭杀毒软件**（临时）：
   - 某些杀毒软件可能阻止文件创建
   - 临时禁用后重试

---

## 🔴 常见问题 5：中文路径问题

### 错误信息
```
编码错误
无法打开文件
```

### 解决方案

**方案 1：避免中文路径（推荐）**
```
❌ C:\用户\张三\文档\minizip-ng
✅ C:\minizip-ng
✅ D:\projects\minizip-ng
```

**方案 2：设置编译器编码**

在 CMakeLists.txt 中添加（已包含）：
```cmake
if(MSVC)
    add_compile_options(/utf-8)
endif()
```

---

## 🔴 常见问题 6：缺少依赖库

### 错误信息
```
Could not find ZLIB
Could not find BZIP2
```

### 解决方案

项目会自动下载依赖库（如果启用了 `MZ_FETCH_LIBS`）。

如果自动下载失败：

**方案 1：禁用可选功能**
```batch
cmake -G "Visual Studio 17 2022" -A x64 ^
      -D MZ_BUILD_TESTS=ON ^
      -D MZ_ZLIB=ON ^
      -D MZ_BZIP2=OFF ^
      -D MZ_LZMA=OFF ^
      -D MZ_ZSTD=OFF ^
      ..
```

**方案 2：手动安装依赖**

使用 vcpkg：
```batch
# 安装 vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat

# 安装依赖
vcpkg install zlib:x64-windows
vcpkg install bzip2:x64-windows
vcpkg install liblzma:x64-windows

# 使用 vcpkg 的 CMake
cmake -G "Visual Studio 17 2022" -A x64 ^
      -D CMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake ^
      -D MZ_BUILD_TESTS=ON ^
      ..
```

---

## 🔴 常见问题 7：编译时内存不足

### 错误信息
```
fatal error C1060: 编译器堆空间不足
```

### 解决方案

**方法 1：减少并行编译数**
```batch
# 只用 2 个 CPU 核心编译
cmake --build . --config Release -- /m:2
```

**方法 2：增加堆空间**
```batch
# 设置环境变量
set _CL_=/Zm200
cmake --build . --config Release
```

---

## 🔴 常见问题 8：LNK2001 未解析的外部符号

### 错误信息
```
error LNK2001: 无法解析的外部符号
```

### 解决方案

1. **确保使用相同的配置**：
   - Debug 库只能链接到 Debug 程序
   - Release 库只能链接到 Release 程序

2. **检查平台**：
   - 确保都是 x64 或都是 x86（不要混用）

3. **重新生成解决方案**：
   ```
   生成 → 清理解决方案
   生成 → 重新生成解决方案
   ```

---

## 🛠️ 完整的手动编译步骤（如果脚本失败）

如果自动脚本完全失败，可以手动执行：

```batch
REM 1. 移动项目到短路径
move "C:\Users\...\minizip-ng-..." "C:\minizip-ng"

REM 2. 打开 Developer Command Prompt for VS 2022
REM    (从开始菜单找到)

REM 3. 导航到项目目录
cd C:\minizip-ng

REM 4. 创建构建目录
mkdir build
cd build

REM 5. 生成 VS 项目
cmake -G "Visual Studio 17 2022" -A x64 -D MZ_BUILD_TESTS=ON ..

REM 6. 编译 (Release)
cmake --build . --config Release

REM 7. 或者打开 Visual Studio
start minizip-ng.sln
```

---

## ✅ 验证安装

编译成功后，运行测试：

```batch
cd build\Release
mz_zip_helper_test.exe C:\Temp\ZipTests
```

期望输出：
```
========================================
Minizip-ng Helper Functions Test Suite
========================================

Test directory: C:\Temp\ZipTests

...

Test Summary:
  Total:  12
  Passed: 12
  Failed: 0

RESULT: All tests PASSED!
```

---

## 📞 获取帮助

如果以上方法都不起作用：

1. **检查 CMake 日志**：
   ```
   build_vs2022\CMakeFiles\CMakeOutput.log
   build_vs2022\CMakeFiles\CMakeError.log
   ```

2. **在 GitHub 提交 issue**：
   - 包含完整错误信息
   - 包含 CMake 版本：`cmake --version`
   - 包含 VS 版本：Visual Studio 2022 版本号
   - 包含 Windows 版本：`winver`

3. **临时解决方案**：
   - 使用 MinGW 或 MSYS2 编译（非 Visual Studio）
   - 使用 WSL (Windows Subsystem for Linux) 编译

---

## 🎯 快速诊断清单

遇到问题时，按顺序检查：

- [ ] 路径长度 < 80 字符？
- [ ] CMake 已安装并在 PATH 中？
- [ ] Visual Studio 2022 已安装？
- [ ] 安装了"C++ 桌面开发"工作负载？
- [ ] 使用 Developer Command Prompt？
- [ ] 路径中没有中文字符？
- [ ] 有写入权限？
- [ ] 杀毒软件未阻止？

---

**最后更新**: 2025-11-26
