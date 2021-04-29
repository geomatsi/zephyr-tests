MAJOR = 0.0.1
MINOR = 1

ZEPHYR_BASE ?= /path/to/zephyrproject

all: images

images: firmware

firmware: application bootloader
	python3 ${ZEPHYR_BASE}/zephyr/scripts/mergehex.py -o output/firmware.hex build_mcuboot/zephyr/zephyr.hex build/zephyr/zephyr.signed.hex

application: dir
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west build -p auto  -b blackpill_f401ce
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west sign -t imgtool -p ${ZEPHYR_BASE}/bootloader/mcuboot/scripts/imgtool.py -- --version ${MAJOR}+${MINOR} --key ${ZEPHYR_BASE}/bootloader/mcuboot/root-rsa-2048.pem
	cp build/zephyr/zephyr.signed.bin output/app.bin
	cp build/zephyr/zephyr.signed.hex output/app.hex

bootloader: dir
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west build -d build_mcuboot -b blackpill_f401ce -s ${ZEPHYR_BASE}/bootloader/mcuboot/boot/zephyr -- -DCONF_FILE=${PWD}/mcuboot/prj.conf -DDTC_OVERLAY_FILE=${PWD}/boards/blackpill_f401ce.overlay
	cp build_mcuboot/zephyr/zephyr.hex output/mcuboot.hex

flash: firmware
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west flash --hex-file output/firmware.hex --runner openocd --config support/openocd.cfg

dir:
	mkdir -p output

clean:
	rm -rf build build_mcuboot output
