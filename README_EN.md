# LumiSDK

**Unified application development framework for LumiOS**

[中文](README.md) | English

Provides a C core API (`liblumiapp`) and multi-language bindings for building LumiOS native applications.

## Project Structure

```
LumiSDK/
├── liblumiapp/             C core library
│   ├── include/lumiapp.h   Public header
│   ├── src/                Implementation (app/view/storage/notify/intent/file/timer/log)
│   └── Makefile
├── bindings/               Language bindings
│   ├── cpp/lumiapp.hpp     C++ RAII wrapper (namespace lumi::)
│   ├── rust/               Rust FFI crate (lumiapp)
│   └── java/               JNI bridge (com.lumios.sdk)
├── toolkit/                High-level UI component library
│   ├── include/lumi_toolkit.h
│   └── src/toolkit.c
├── examples/               Sample apps (C, C++)
├── tests/test_sdk.c        Unit tests (14 tests)
└── pkg-config/lumiapp.pc   pkg-config file
```

## Supported Languages

| Language | Binding | Package |
|----------|---------|---------|
| C | Direct call | `liblumiapp` |
| C++ | namespace `lumi::` | `liblumiapp` + `lumiapp.hpp` |
| Rust | FFI crate | `lumiapp` |
| Java/Kotlin | JNI | `com.lumios.sdk` |
| LumiScript | Built-in | (auto-linked) |

## Requirements

| Dependency | Min Version | Notes |
|-----------|-------------|-------|
| GCC / Clang | GCC 11+ / Clang 14+ | C11 compiler |
| GNU Make | 4.0+ | Build tool |
| ar | — | Static archive (bundled with GCC) |

> **Optional (language bindings):**
> - **C++**: G++ 11+ or Clang++ 14+
> - **Rust**: rustc 1.70+, cargo
> - **Java**: JDK 11+

## Setup

### Windows

1. Download [MinGW-w64 GCC 13.2.0 (WinLibs)](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0posix-18.1.5-11.0.1-ucrt-r5/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64ucrt-11.0.1-r5.7z)
2. Extract to `C:\mingw64`, add `C:\mingw64\bin` to PATH
3. Copy `mingw32-make.exe` → `make.exe`

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y build-essential
```

## Build

```bash
cd liblumiapp
make              # Build liblumiapp.so + liblumiapp.a
make test         # Run 14 unit tests
make install      # Install to /usr/local
```

## API Overview

```c
#include <lumiapp.h>

lumi_app_t *lumi_app_create(manifest, lifecycle, userdata);
int lumi_app_run(app);

lumi_view_t *lumi_column();
lumi_view_t *lumi_text("Hello");
lumi_view_t *lumi_button("Click");
lumi_view_add_child(parent, child);
lumi_view_on_click(view, callback, userdata);

lumi_log(LUMI_LOG_INFO, "tag", "message");
lumi_storage_set("key", "value");
lumi_notify_simple("title", "body");
```

## Related Projects

| Project | Description |
|---------|------------|
| [LumiOS](https://github.com/ZephyrZeno/LumiOS) | Operating system core |
| [LumiPkg](https://github.com/ZephyrZeno/LumiPkg) | Package manager |
| [LumiScript](https://github.com/ZephyrZeno/LumiScript) | Programming language |

## License

Apache 2.0
