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

## Flash

Flash using west with OpenOCD runner:
```bash
$ ZEPHYR_BASE=/path/to/zephyr/zephyr west flash --runner openocd
```

## Debug

Debug using west and gdb using OpenOCD runner:
```bash
$ ZEPHYR_BASE=/path/to/zephyr/zephyr west debug --runner openocd
```

## Monitor RTT logs

Monitor RTT logs using JLink RTT viewer:
```bash
$ JLinkRTTViewerExe
```

Monitor RTT logs using [probe-rs-rtt](https://github.com/probe-rs/probe-rs-rtt) tool:
```bash
$ rtthost --chip nRF52832_xxAA -l                                                                                                                                                                                                                                  127 ↵
Attaching to RTT...
Up channels:
  0: Terminal (buffer size 4096)
Down channels:
  0: Terminal (buffer size 16)

$ rtthost --chip nRF52832_xxAA -u 0
Attaching to RTT...
Found control block at 0x2000038c
cycle 4841: led1
cycle 4842: led2
cycle 4843: led3
cycle 4844: led0
cycle 4845: led1
cycle 4846: led2
...
```
