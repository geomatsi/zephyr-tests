/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>

#include <sys/printk.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#define SLEEP_TIME_MS   100

#define LED_NODE0 DT_NODELABEL(led0)
#define LED_NODE1 DT_NODELABEL(led1)
#define LED_NODE2 DT_NODELABEL(led2)
#define LED_NODE3 DT_NODELABEL(led3)

#define LED0	DT_GPIO_LABEL(LED_NODE0, gpios)
#define LED1	DT_GPIO_LABEL(LED_NODE1, gpios)
#define LED2	DT_GPIO_LABEL(LED_NODE2, gpios)
#define LED3	DT_GPIO_LABEL(LED_NODE3, gpios)

#define PIN0	DT_GPIO_PIN(LED_NODE0, gpios)
#define PIN1	DT_GPIO_PIN(LED_NODE1, gpios)
#define PIN2	DT_GPIO_PIN(LED_NODE2, gpios)
#define PIN3	DT_GPIO_PIN(LED_NODE3, gpios)

#define FLAGS0	DT_GPIO_FLAGS(LED_NODE0, gpios)
#define FLAGS1	DT_GPIO_FLAGS(LED_NODE1, gpios)
#define FLAGS2	DT_GPIO_FLAGS(LED_NODE2, gpios)
#define FLAGS3	DT_GPIO_FLAGS(LED_NODE3, gpios)

void main(void)
{
	const struct device *dev;
	bool led_is_on = true;
	uint32_t c = 0;
	int ret;

        printk("Hello World! %s\n", CONFIG_BOARD);

	dev = device_get_binding(LED0);
	if (dev == NULL) {
		printk("failed to bind LED0\n");
		return;
	}

	ret = gpio_pin_configure(dev, PIN0, GPIO_OUTPUT_ACTIVE | FLAGS0);
	if (ret < 0) {
		printk("failed to configure LED0\n");
		return;
	}

	while (1) {
		gpio_pin_set(dev, PIN0, (int)led_is_on);
		led_is_on = !led_is_on;
		k_msleep(SLEEP_TIME_MS);
		printk("blink %u\n", c++);
	}
}
