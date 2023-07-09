#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nmea_parser.h"

LOG_MODULE_REGISTER(Speedometer);

/* SSD1306 Display */
#define DISPLAY_BUFFER_PITCH 128
static const struct device *display_dev = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

/* UART */
#define UART_NODE1 DT_NODELABEL(uart0)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_NODE1);
#define MSG_SIZE 82
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 128); // Meassage queue for UART

void uart_cb(const struct device *dev, void *user_data) {
	ARG_UNUSED(user_data);

	uint8_t c;
	int rx_buf_pos = 0;
	char rx_buf[MSG_SIZE]; // 80 + 2 for \r\n; maximum length according to NMEA 0183 standard

	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

int uart_init() {
	if (uart_dev == NULL) {
		LOG_ERR("uart device pointer is NULL");
		return -1;
	}

	if (!device_is_ready(uart_dev)) {
		LOG_ERR("uart device is not ready");
		return -1;
	}

	int ret = uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);

	if (ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
			return -ENOTSUP;
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
			return -ENOSYS;
		} else {
			printk("Error setting UART callback: %d\n", ret);
			return ret;
		}
		return 0;
	}
	uart_irq_rx_enable(uart_dev);

	LOG_INF("UART-Device Name: %s", uart_dev->name);

	return 0;
}

int display_init() {
	if (display_dev == NULL) {
		LOG_ERR("display device pointer is NULL");
		return -1;
	}

	if (!device_is_ready(display_dev)) {
		LOG_ERR("display device is not ready");
		return -1;
	}

	if (display_set_pixel_format(display_dev, PIXEL_FORMAT_MONO10) != 0)
	{
		LOG_ERR("Failed to set required pixel format\n");
		return -1;
	}

	if (cfb_framebuffer_init(display_dev))
	{
		LOG_ERR("Framebuffer initialization failed!\n");
		return -1;
	}

	/* DISPLAY INIT */
	cfb_framebuffer_clear(display_dev, true);
	display_blanking_off(display_dev);
	struct display_capabilities capabilities;
	display_get_capabilities(display_dev, &capabilities);

	const uint16_t x_res = capabilities.x_resolution;
	const uint16_t y_res = capabilities.y_resolution;

	LOG_INF("x_resolution: %d", x_res);
	LOG_INF("y_resolution: %d", y_res);
	LOG_INF("supported pixel formats: %d", capabilities.supported_pixel_formats);
	LOG_INF("screen_info: %d", capabilities.screen_info);
	LOG_INF("current_pixel_format: %d", capabilities.current_pixel_format);
	LOG_INF("current_orientation: %d", capabilities.current_orientation);

	const struct display_buffer_descriptor buf_desc = {
		.width = x_res,
		.height = y_res,
		.buf_size = x_res * y_res,
		.pitch = DISPLAY_BUFFER_PITCH
	};

	cfb_framebuffer_clear(display_dev, false);
	/* END DISPLAY INIT*/

	return 0;
}

int initialize_devices(void) {
	if (display_init()) {
		LOG_ERR("Failed to initialize display");
		return;
	}

	if (uart_init()) {
		LOG_ERR("Failed to initialize uart");
		return;
	}

	return 0;
}

void display_print_speed(struct device *dev, uint8_t brightness, char *buf) {
	if (display_set_contrast(dev, brightness)) {
		LOG_ERR("could not set display contrast");
	}
	cfb_framebuffer_clear(dev, false);
	cfb_framebuffer_set_font(dev, 0);
	cfb_print(dev, buf, 0, 0);
	cfb_framebuffer_finalize(dev);
}

void main(void) {
	struct nmea_data nmea_data;
	struct gprmc_data gprmc;
    nmea_data.gprmc = gprmc;
	struct gpvtg_data gpvtg;
    nmea_data.gpvtg = gpvtg;
	struct gpgga_data gpgga;
	nmea_data.gpgga = gpgga;
	
	uint8_t tx_buf[MSG_SIZE];
	int ret = 0;

	LOG_INF("Starting speedometer");

	if (initialize_devices()) {
		LOG_ERR("Failed to initialize devices");
		return;
	}
	
	while (k_msgq_get(&uart_msgq, &tx_buf, K_FOREVER) == 0) {
		ret = parse_nmea(tx_buf, &nmea_data);
		if (ret) {
			LOG_ERR("Failed to parse NMEA message");
			continue;
		}
		display_print_speed(display_dev, 255, gpvtg.ground_speed_kph);
	}

}