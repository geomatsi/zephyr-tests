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

struct led_desc {
	const struct device *led;
	char *name;
	char *label;
	gpio_pin_t pin;
	gpio_flags_t flags;
	bool ready;
};

struct led_desc leds_list[] = {
	/* LED0 */
	{
		.name	= "led0",
		.label	= DT_GPIO_LABEL(DT_NODELABEL(led0), gpios),
		.pin	= DT_GPIO_PIN(DT_NODELABEL(led0), gpios),
		.flags	= DT_GPIO_FLAGS(DT_NODELABEL(led0), gpios),
		.ready	= false,
	},
	/* LED1 */
	{
		.name	= "led1",
		.label	= DT_GPIO_LABEL(DT_NODELABEL(led1), gpios),
		.pin	= DT_GPIO_PIN(DT_NODELABEL(led1), gpios),
		.flags	= DT_GPIO_FLAGS(DT_NODELABEL(led1), gpios),
		.ready	= false,
	},
	/* LED2 */
	{
		.name	= "led2",
		.label	= DT_GPIO_LABEL(DT_NODELABEL(led2), gpios),
		.pin	= DT_GPIO_PIN(DT_NODELABEL(led2), gpios),
		.flags	= DT_GPIO_FLAGS(DT_NODELABEL(led2), gpios),
		.ready	= false,
	},
	/* LED3 */
	{
		.name	= "led3",
		.label	= DT_GPIO_LABEL(DT_NODELABEL(led3), gpios),
		.pin	= DT_GPIO_PIN(DT_NODELABEL(led3), gpios),
		.flags	= DT_GPIO_FLAGS(DT_NODELABEL(led3), gpios),
		.ready	= false,
	},
};

void main(void)
{
	uint32_t cycle = 0;
	struct led_desc *p;
	int ret;
	int i;

        printk("Hello World! %s\r\n", CONFIG_BOARD);

	for (i = 0; i < ARRAY_SIZE(leds_list); i++) {
		p = &leds_list[i];
		p->led = device_get_binding(p->label);
		if (!p->led) {
			printk("failed to bind %s\r\n", p->name);
			continue;
		}

		ret = gpio_pin_configure(p->led, p->pin, p->flags | GPIO_OUTPUT_ACTIVE);
		if (ret) {
			printk("failed to configure %s\r\n", p->name);
			continue;
		}

		gpio_pin_set(p->led, p->pin, 0);
		p->ready = true;
	}

	while (1) {
		p = &leds_list[cycle % ARRAY_SIZE(leds_list)];

		if (p->ready)
			gpio_pin_toggle(p->led, p->pin);

		printk("cycle %u: %s\r\n", cycle++, p->name);
		k_msleep(SLEEP_TIME_MS);
	}
}
