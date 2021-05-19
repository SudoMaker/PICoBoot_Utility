# PICoBoot Utility

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
