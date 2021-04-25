MAJOR = 0.0.1
MINOR = 1

ZEPHYR_BASE ?= /path/to/zephyrproject

all: images

images: firmware

firmware: application bootloader
	python3 ${ZEPHYR_BASE}/zephyr/scripts/mergehex.py -o output/firmware.hex build_mcuboot/zephyr/zephyr.hex build/zephyr/zephyr.signed.hex

application: dir
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west build -p auto  -b blackpill
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west sign -t imgtool -p ${ZEPHYR_BASE}/bootloader/mcuboot/scripts/imgtool.py -- --version ${MAJOR}+${MINOR}
	cp build/zephyr/zephyr.signed.bin output/app.bin
	cp build/zephyr/zephyr.signed.hex output/app.hex

bootloader: dir
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west build -d build_mcuboot -s ${ZEPHYR_BASE}/bootloader/mcuboot/boot/zephyr -- -DBOARD=blackpill -DBOARD_ROOT=${PWD} -DCONF_FILE=${PWD}/mcuboot/prj.conf
	cp build_mcuboot/zephyr/zephyr.hex output/mcuboot.hex

flash: firmware
	ZEPHYR_BASE=${ZEPHYR_BASE}/zephyr west flash --hex-file output/firmware.hex

dir:
	mkdir -p output

clean:
	rm -rf build build_mcuboot output
