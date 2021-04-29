/*
 * Copyright (c) 2021 Sergey Matyukevich <geomatsi@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>

#include <sys/printk.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#define SLEEP_TIME_US 500000

#define LED_NODE DT_NODELABEL(user_led)

#define LED	DT_GPIO_LABEL(LED_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED_NODE, gpios)


void main(void)
{
	const struct device *dev;
	uint32_t cycle = 0;
	int ret;

        printk("Hello World!\r\n");

	dev = device_get_binding(LED);
	if (dev == NULL) {
		printk("failed to get binding...\r\n");
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		printk("failed to configure...\r\n");
		return;
	}

	while (1) {
		printk("cycle %u\r\n", cycle++);
		gpio_pin_toggle(dev, PIN);
		k_busy_wait(SLEEP_TIME_US);
	}
}
