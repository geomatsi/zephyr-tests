/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <string.h>
#include <stdlib.h>

#include <sys/printk.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <nrfx_nfct.h>
#include <nrfx_timer.h>

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

#define NFCT_BUFFER_SIZE	540

static uint8_t frame[NFCT_BUFFER_SIZE] = { 0 };
static bool rx_pending  = false;
static bool selected = false;

void frame_rx_start()
{
	if (rx_pending) {
		printk("RX: in progress...\r\n");
		return;
	}

	memset(frame, 0, NFCT_BUFFER_SIZE);
	rx_pending = true;

	nrfx_nfct_data_desc_t data = {
		.data_size = NFCT_BUFFER_SIZE,
		.p_data = frame,
	};

	nrfx_nfct_rx(&data);
}

void frame_rx_end(const nrfx_nfct_evt_rx_frameend_t *rx_frameend)
{
	const uint8_t *data = rx_frameend->rx_data.p_data;
	size_t size = rx_frameend->rx_data.data_size;

	if (!rx_pending) {
		//printk("RX: %d bytes frame dropped\r\n", size);
		return;
	}

	rx_pending = false;

	if (size == 0) {
		printk("RX: empty frame ?\r\n");
		return;
	}

	printk("RX: command 0x%02x\r\n", data[0]);
	// TODO: process command
}

void frame_error(const nrfx_nfct_evt_error_t *error)
{
	switch (error->reason) {
	case NRFX_NFCT_ERROR_FRAMEDELAYTIMEOUT:
		printk("ERR: frame delay timeout\r\n");
		break;
	default:
		printk("ERR: unknown reason: %d\r\n", error->reason);
		break;
	}
}

void nfct_notifier(nrfx_nfct_evt_t const *e)
{
	switch (e->evt_id) {
	case NRFX_NFCT_EVT_FIELD_DETECTED:
		//printk("EVT: field detected\r\n");
		nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);
		break;
	case NRFX_NFCT_EVT_FIELD_LOST:
		//printk("EVT: field lost\r\n");
		selected = false;
		break;
	case NRFX_NFCT_EVT_SELECTED:
		printk("EVT: tag selected\r\n");
		selected = true;
		frame_rx_start();
		break;
	case NRFX_NFCT_EVT_RX_FRAMESTART:
		//printk("EVT: frame rx start\r\n");
		break;
	case NRFX_NFCT_EVT_RX_FRAMEEND:
		//printk("EVT: frame rx end\r\n");
		frame_rx_end(&e->params.rx_frameend);
		break;
	case NRFX_NFCT_EVT_TX_FRAMESTART:
		printk("EVT: frame tx start\r\n");
		break;
	case NRFX_NFCT_EVT_TX_FRAMEEND:
		printk("EVT: frame tx end\r\n");
		frame_rx_start();
		break;
	case NRFX_NFCT_EVT_ERROR:
		frame_error(&e->params.error);
		break;
	default:
		printk("EVT: unknown event %u\r\n", e->evt_id);
		break;
	}
}

const nrfx_nfct_config_t nfct_config = {

	.rxtx_int_mask = NRFX_NFCT_EVT_FIELD_DETECTED | NRFX_NFCT_EVT_FIELD_LOST |
		NRFX_NFCT_EVT_SELECTED | NRFX_NFCT_EVT_RX_FRAMESTART |
		NRFX_NFCT_EVT_RX_FRAMEEND | NRFX_NFCT_EVT_TX_FRAMESTART |
		NRFX_NFCT_EVT_TX_FRAMEEND | NRFX_NFCT_EVT_ERROR, 
	.cb = nfct_notifier,
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

	IRQ_CONNECT(TIMER4_IRQn, NRFX_NFCT_DEFAULT_CONFIG_IRQ_PRIORITY, nrfx_isr, nrfx_timer_4_irq_handler, 0);
	IRQ_CONNECT(NFCT_IRQn, NRFX_NFCT_DEFAULT_CONFIG_IRQ_PRIORITY, nrfx_isr, nrfx_nfct_irq_handler, 0);

	ret = nrfx_nfct_init(&nfct_config);
	if (ret != NRFX_SUCCESS) {
		printk("NFCT init failed: %d", ret);
		return;
	}

	nrfx_nfct_enable();

	while (1) {
		p = &leds_list[cycle++ % ARRAY_SIZE(leds_list)];

		if (p->ready)
			gpio_pin_toggle(p->led, p->pin);

		k_msleep(SLEEP_TIME_MS);
	}
}
