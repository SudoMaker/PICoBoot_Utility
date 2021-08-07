# PICoBoot Utility

[![Build Status](https://github.com/SudoMaker/PICoBoot_Utility/workflows/Build/badge.svg)](https://github.com/SudoMaker/PICoBoot_Utility/actions/workflows/push_pr_build_cmake.yml) [![Release Status](https://github.com/SudoMaker/PICoBoot_Utility/workflows/Release/badge.svg)](https://github.com/SudoMaker/PICoBoot_Utility/actions/workflows/release_cmake.yml)

Utility for the [PICoBoot bootloader](https://github.com/SudoMaker/PICoBoot).

## Usage
Just like the famous `fastboot`.

```shell
picoboot devices
```

```shell
picoboot info
```

```shell
picoboot reboot
```

```shell
picoboot flash /foo/bar.hex
```

Read help for more usages.

## Build
### Requirements
- Working Internet connection (for package downloading)
- CMake 3.14+
- C++17 compatible compiler
- POSIX environment (If you can't live without Windows, please use Cygwin or WSL)

## Binary Releases
Automatically built targets: 
- Linux x86_64
- MacOS x86_64

See [releases](https://github.com/SudoMaker/PICoBoot_Utility/releases).