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
#define SW0_NODE DT_ALIAS(sw0)

#define LED	DT_GPIO_LABEL(LED_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED_NODE, gpios)

#define SW0_GPIO_LABEL	DT_GPIO_LABEL(SW0_NODE, gpios)
#define SW0_GPIO_PIN	DT_GPIO_PIN(SW0_NODE, gpios)
#define SW0_GPIO_FLAGS	(GPIO_INPUT | DT_GPIO_FLAGS(SW0_NODE, gpios))

static struct gpio_callback button_cb_data;

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

void main(void)
{
	const struct device *button;
	const struct device *led;
	uint32_t cycle = 0;
	int ret;

        printk("Hello World!\r\n");

	/* Configure LED */

	led = device_get_binding(LED);
	if (led == NULL) {
		printk("Error: didn't find %s device\n", LED);
		return;
	}

	ret = gpio_pin_configure(led, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, LED, PIN);
		return;
	}

	/* Configure user button */

	button = device_get_binding(SW0_GPIO_LABEL);
	if (button == NULL) {
		printk("Error: didn't find %s device\n", SW0_GPIO_LABEL);
		return;
	}

	ret = gpio_pin_configure(button, SW0_GPIO_PIN, SW0_GPIO_FLAGS);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, SW0_GPIO_LABEL, SW0_GPIO_PIN);
		return;
	}

	ret = gpio_pin_interrupt_configure(button, SW0_GPIO_PIN, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, SW0_GPIO_LABEL, SW0_GPIO_PIN);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(SW0_GPIO_PIN));
	gpio_add_callback(button, &button_cb_data);

	while (1) {
		printk("cycle %u\r\n", cycle++);
		gpio_pin_toggle(led, PIN);
		k_busy_wait(SLEEP_TIME_US);
	}
}
