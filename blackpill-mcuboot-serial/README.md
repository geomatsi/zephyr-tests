# MCUboot bootloader

This example demonstrates the use of MCUboot bootloader and serial firmware update with two different applicatons:
* Zephyr application
* Embedded Rust application

Note: minimal supported Zephyr version is v2.5.0

## Zephyr firmware

This section provides a brief overview of steps required to build MCUboot bootloader and Zephyr application for Blackpill board. It is assumed that Zephyr source tree is located at /path/to/zephyr/project/root.

### Build and sign Zephyr application

Build and sign Zephyr application:

```bash
$ ZEPHYR_BASE=/path/to/zephyr/project/root/zephyr west build -p auto  -b blackpill
$ ZEPHYR_BASE=/path/to/zephyr/project/root/zephyr west sign -t imgtool -p /path/to/zephyr/project/root/bootloader/mcuboot/scripts/imgtool.py -- --key /path/to/zephyr/project/root/bootloader/mcuboot/root-rsa-2048.pem
$ ls -la build/zephyr/zephyr.signed.*
-rw-rw-r-- 1 user user 90956  Feb 1 15:21 build/zephyr/zephyr.signed.bin
-rw-rw-r-- 1 user user 250176 Feb 1 15:21 build/zephyr/zephyr.signed.hex
```

### Build MCUboot bootloader

Build MCUBoot bootloader:

```bash
$ ZEPHYR_BASE=/path/to/zephyr/project/root/zephyr west build -d build_mcuboot -b blackpill_f401ce -s /path/to/zephyr/project/root/bootloader/mcuboot/boot/zephyr -- -DCONF_FILE=${PWD}/mcuboot/prj.conf -DDTC_OVERLAY_FILE=${PWD}/boards/blackpill_f401ce.overlay
```

### Generate factory image

It is possible to merge two hex images into a single image. This option is convenient for initial hardware bootstrap when single hex image can be flashed to the board via SWD:

```bash
$ mkdir output
$ python3 /path/to/zephyr/project/root/zephyr/scripts/mergehex.py -o output/firmware.hex build_mcuboot/zephyr/zephyr.hex build/zephyr/zephyr.signed.hex
```

### Flash factory image

Write factory image to flash using OpenOCD/JLink:

```bash
$ ZEPHYR_BASE=/path/to/zephyr/project/root/zephyr west flash --hex-file output/firmware.hex --runner openocd --config support/openocd.cfg
```

### Using Makefile

Project provides Makefile to simplify building images and to track release versions:

```bash
$ ZEPHYR_BASE=/path/to/zephyr/project/root make clean
$ ZEPHYR_BASE=/path/to/zephyr/project/root make firmware
$ ZEPHYR_BASE=/path/to/zephyr/project/root make flash
```

Targets:
* application: signed and unsigned Zephyr sample application
* bootloader: MCUBoot bootloader
* firmware: combined hex file with both bootloader and signed application
* flash: build and flash firmware

Images are stored in 'output' directory:
* app.hex, app.bin: signed Zephyr application
* mcuboot.hex: MCUBoot bootloader
* firmware.hex: combined HEX image including both MCUBoot bootloader and signed Zephyr application

## Firmware update using MCUboot

MCUboot bootloader in this example is configured to support firmware update over serial connection.  Use [mcumgr-cli](https://github.com/apache/mynewt-mcumgr-cli)
command line tool to update firmware over serial.

Here are the steps:

1. Connect USB-to-serial adapter
2. Press KEY button and reset the board
3. Check current image on the board
```bash
$ mcumgr --conntype serial --connstring dev=/dev/ttyUSB0,baud=115200,mtu=512  image list
Images:
 image=0 slot=0
    version: 0.0.1.1
    bootable: false
    flags:
    hash: Unavailable
Split status: N/A (0)
```
4. Update image on the board
```bash
$ mcumgr --conntype serial --connstring dev=/dev/ttyUSB0,baud=115200,mtu=512 image upload output/app.bin
 19.66 KiB / 19.66 KiB [==========================================================================================================================================================================================================================================] 100.00% 1.31 KiB/s 14s
Done
```

5. Reset the board
```bash
$ mcumgr --conntype serial --connstring dev=/dev/ttyUSB0,baud=115200,mtu=512 reset
Done
```

## Monitor RTT logs

Monitor RTT logs using [probe-rs-rtt](https://github.com/probe-rs/probe-rs-rtt) tool:

```bash
$ rtthost  -c STM32F401CEUx -p 0 --scan-region 0x20000000..0x20001000  -l
Attaching to RTT...
Up channels:
  0: Terminal (buffer size 4096)
Down channels:
  0: Terminal (buffer size 16)

$ rtthost  -c STM32F401CEUx -p 0 --scan-region 0x20000000..0x20001000
Attaching to RTT...
Found control block at 0x200002f0
*** Booting Zephyr OS build zephyr-v2.4.0  ***
Hello World!
cycle 0
cycle 1
cycle 2
cycle 3
```

## Alternative application
In addition to Zephyr application this example provides alternative application compatible with MCUboot. This application is written in Rust.
Here is a brief overview of steps required to build embedded Rust application and to supply it as a firmware update over serial connection:

1. Build Rust application
```bash
$ cd alt
$ ZEPHYR_BASE=/path/to/zephyr/project/root cargo make app
```
2. Press KEY button, reset the board, and update firmware
```bash
$ mcumgr --conntype serial --connstring dev=/dev/ttyUSB0,baud=115200,mtu=512 image upload test-rs-fw.signed.bin
```
3. Reboot into new firmware
```bash
$ mcumgr --conntype serial --connstring dev=/dev/ttyUSB0,baud=115200,mtu=512 reset
```
4. Check RTT logs
```bash
$ rtthost  -c STM32F401CEUx -p 0 --scan-region 0x20000000..0x20001000
      Attaching to RTT...
      Found control block at 0x20000028
      idle loop: 0
      tim2 irq
      idle loop: 1
      tim2 irq
      idle loop: 2
      idle loop: 3
```
