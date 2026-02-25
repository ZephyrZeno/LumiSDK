# LumiSDK

**LumiOS 统一应用开发框架**

[English](README_EN.md) | 中文

提供 C 核心 API (`liblumiapp`) 和多语言绑定，用于构建 LumiOS 原生应用。

## 项目结构

```
LumiSDK/
├── liblumiapp/             C 核心库
│   ├── include/lumiapp.h   公共头文件
│   ├── src/                实现 (app/view/storage/notify/intent/file/timer/log)
│   ├── build/              构建输出
│   └── Makefile
├── bindings/               语言绑定
│   ├── cpp/lumiapp.hpp     C++ RAII wrapper (namespace lumi::)
│   ├── rust/               Rust FFI crate (lumiapp)
│   └── java/               JNI bridge (com.lumios.sdk)
├── toolkit/                高级 UI 组件库 (lumi-toolkit)
│   ├── include/lumi_toolkit.h
│   └── src/toolkit.c
├── examples/               各语言示例
│   ├── hello_c/            C 示例应用
│   └── hello_cpp/          C++ 示例应用
├── tests/test_sdk.c        单元测试 (14 tests)
└── pkg-config/lumiapp.pc   pkg-config 配置
```

## 支持的语言

| 语言 | 绑定方式 | 包/库名 |
|------|---------|---------|
| C | 直接调用 | `liblumiapp` |
| C++ | namespace `lumi::` | `liblumiapp` + `lumiapp.hpp` |
| Rust | FFI crate | `lumiapp` |
| Java/Kotlin | JNI | `com.lumios.sdk` |
| LumiScript | 编译器内置 | (自动链接) |

## 环境要求

| 依赖 | 最低版本 | 说明 |
|------|---------|------|
| GCC / Clang | GCC 11+ / Clang 14+ | C11 标准编译器 |
| GNU Make | 4.0+ | 构建工具 |
| ar | — | 静态库归档工具 (随 GCC / binutils 安装) |

> **可选依赖（语言绑定）：**
> - **C++**: G++ 11+ 或 Clang++ 14+
> - **Rust**: rustc 1.70+, cargo（`cargo build` 自动链接 liblumiapp）
> - **Java**: JDK 11+（编译 JNI 桥接库需要 `$JAVA_HOME/include`）

## 环境安装

### Windows

1. **下载 MinGW-w64 GCC 13.2.0**:
   - [WinLibs GCC 13.2.0 (x86_64, UCRT)](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0posix-18.1.5-11.0.1-ucrt-r5/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64ucrt-11.0.1-r5.7z)
   - 备选: [MinGW-Builds GCC 13.2.0](https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev1.7z)

2. **解压到** `C:\mingw64`（确保 `C:\mingw64\bin\gcc.exe` 存在）

3. **添加 PATH**: 系统设置 → 环境变量 → Path 添加 `C:\mingw64\bin`

4. **创建 make 别名**: 复制 `C:\mingw64\bin\mingw32-make.exe` → `C:\mingw64\bin\make.exe`

5. **验证**:
   ```powershell
   gcc --version    # 应输出 gcc 13.2.0
   make --version   # 应输出 GNU Make 4.x
   ```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y build-essential    # gcc, g++, make, ar
```

验证:
```bash
gcc --version     # 应输出 gcc 11+
make --version    # 应输出 GNU Make 4.x
```

### 可选: Rust 绑定环境

```bash
# Windows: 下载 https://rustup.rs 安装 rustup
# Ubuntu:
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

### 可选: Java 绑定环境

```bash
# Windows: 下载 https://adoptium.net/temurin/releases/ (JDK 17 LTS)
# Ubuntu:
sudo apt install -y openjdk-17-jdk
```

## 构建

### 编译核心库 (liblumiapp)

```bash
cd liblumiapp
make              # 生成 build/liblumiapp.so + build/liblumiapp.a
make install      # 安装到 /usr/local/lib + /usr/local/include
```

**Windows (PowerShell)**:
```powershell
cd liblumiapp
make              # 生成 build\liblumiapp.a
```

### 运行测试

```bash
cd liblumiapp
make test         # 编译并运行 14 个单元测试
```

**Windows 手动编译测试 (从项目根目录执行)**:
```powershell
mkdir liblumiapp\build -Force
cd liblumiapp
gcc -Wall -O2 -std=c11 -Iinclude -c src\log.c src\app.c src\view.c src\storage.c src\notify.c src\intent.c src\file.c src\timer.c
move *.o build\
ar rcs build\liblumiapp.a build\*.o
cd ..
gcc -Wall -O2 -std=c11 -Iliblumiapp\include -o liblumiapp\build\test_sdk.exe tests\test_sdk.c -Lliblumiapp\build -llumiapp
.\liblumiapp\build\test_sdk.exe
```

### 编译 Toolkit

```bash
gcc -Wall -O2 -std=c11 -Iliblumiapp/include -Itoolkit/include \
    -c toolkit/src/toolkit.c -o toolkit/build/toolkit.o
```

### 编译示例

```bash
# C 示例
gcc -Wall -O2 -std=c11 -Iliblumiapp/include -Itoolkit/include \
    -o build/hello examples/hello_c/main.c toolkit/src/toolkit.c \
    -Lliblumiapp/build -llumiapp

# C++ 示例
g++ -Wall -O2 -std=c++17 -Iliblumiapp/include -Ibindings/cpp \
    -o build/hello_cpp examples/hello_cpp/main.cpp \
    -Lliblumiapp/build -llumiapp
```

## 核心 API 概览

```c
#include <lumiapp.h>

// 应用生命周期
lumi_app_t *lumi_app_create(manifest, lifecycle, userdata);
int lumi_app_run(app);

// UI 视图
lumi_view_t *lumi_column();
lumi_view_t *lumi_text("Hello");
lumi_view_t *lumi_button("Click");
lumi_view_add_child(parent, child);
lumi_view_on_click(view, callback, userdata);

// 系统服务
lumi_log(LUMI_LOG_INFO, "tag", "message");
lumi_storage_set("key", "value");
lumi_notify_simple("title", "body");
```

## 相关项目

| 项目 | 说明 |
|------|------|
| [LumiOS](https://github.com/ZephyrZeno/LumiOS) | LumiOS 操作系统核心 |
| [LumiPkg](https://github.com/ZephyrZeno/LumiPkg) | 包管理器 (lmpkg + lmpk-build) |
| [LumiScript](https://github.com/ZephyrZeno/LumiScript) | LumiScript 声明式开发语言 |

## License

Apache 2.0
