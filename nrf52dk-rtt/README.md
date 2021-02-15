# Overview

## Build
It is assumed that Zephyr is properly installed in the workspace at the following location:
```bash
$ ls /path/to/zephyr/
  bootloader  modules  tools  zephyr
```

Build using west tool:
```bash
$ ZEPHYR_BASE=/path/to/zephyr/zephyr west build -p auto  -b nrf52dk_nrf52832
```

# Flash

Flash using west and OpenOCD:
```bash
$ ZEPHYR_BASE=/path/to/zephyr/zephyr west flash --runner openocd
```

