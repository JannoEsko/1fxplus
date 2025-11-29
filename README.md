# 1fxplus — Modern gametype mod for SoF2Plus engine

**1fxplus** is a complete, from-the-ground-up gametype mod for **Soldier of Fortune II** that recreates the classic *1fxmod* gameplay while being developed specifically for the **SoF2Plus engine**.

This is **not a direct port** of the original 1fxmod — it is a modern reimplementation that preserves the original feel and feature set, while fully leveraging:

- Multiprotocol engine support
- Modern, native DLL / SO modules
- Full x86-64 support

---

## ✅ Requirements

### Engine (Mandatory)

[**SoF2Plus MP-MV** on Github](https://github.com/JannoEsko/sof2plus-engine)

Please also ensure that you read through sof2plus-engine readme, as 1fxplus will not function without sof2plus.

`1fxplus` will **not** run on the original Raven `sof2ded` executable.

Supported versions:
- Gold (1.03)
- Silver (1.00)

Both work thanks to SoF2Plus **multiprotocol support**.

---

## 🚀 Features

- ✅ Full multiprotocol support (Gold + Silver players on same server)
  - Automatic weapon disabling per protocol
- 🎮 Gametypes brought to you by the original 1fxmod
- 🧩 Extra gametypes:
  - CSINF
  - Prophunt
  - Kill the VIP
  - Gungame
- 📥 Smart downloads
- 🗄 SQLite logging
- 🔌 Socket logging
- 🌍 IPHub country / VPN detection
- 🗳 Voting system
- 🛡 Hybrid anticamp system

---

## ▶ Quick Server Start (example with x86_64)

### Linux / macOS
```bash
./sof2plus.x86_64 +set dedicated 2 +set fs_game "1fx" +set fs_useDefaultHomePath "0"
```

### Windows
Same command — just use `.exe` as the extension

---

## ⚙ Configuration

### Recommended Config

We have provided a configuration file which gives you all the information how to properly set up a 1fxplus + sof2plus server. [You can find the configuration file here: /1fx/s2pconfig.cfg](https://github.com/JannoEsko/1fxplus/blob/master/1fx/s2pconfig.cfg)


## 🛠 Building

1fxplus is built as a **native DLL / SO module** using a modern ABI with full 32-bit and 64-bit support.

### Build Instructions with CMake

These examples do not specify toolset for build (e.g. ninja), but feel free to use different toolsets.
```bash
# 32-bit Debug build
cmake -S . -B build-x86-debug -DCMAKE_BUILD_TYPE=Debug -DFORCE_32BIT=ON
cmake --build build-x86-debug

# 64-bit Release build
cmake -S . -B build-x86_64-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-x86_64-release
```

---

### MSVC / Visual Studio

The project includes .sln files to enable solution builds in Visual Studio. For Windows builds, we encourage you to use MSVC builds instead of MSYS2 due to DbgHelp library.

## 🔗 Links

### Required Engine
[sof2plus-engine by JannoEsko on GitHub](https://github.com/JannoEsko/sof2plus-engine)

### Original 1fxmod Reference
[1fxmod on GitHub](https://github.com/1fx/1fxmod)

### Original sof2plus Engine and Game module
[sof2plus-engine by AJSchat on GitHub](https://github.com/sof2plus/sof2plus-engine)

[sof2plus-engine by AJSchat on GitHub](https://github.com/sof2plus/sof2plus-game)

---
