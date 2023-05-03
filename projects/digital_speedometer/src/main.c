#include <stdint.h>
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

#include "cfb_font_speed_bold.h"

#define DISPLAY_BUFFER_PITCH 128

LOG_MODULE_REGISTER(Speedometer);

static const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

void main(void)
{
	char buf[32];
  if (dev == NULL) {
    LOG_ERR("device pointer is NULL");
    return;
  }

  if (!device_is_ready(dev)) {
    LOG_ERR("display device is not ready");
    return;
  }

	if (display_set_pixel_format(dev, PIXEL_FORMAT_MONO10) != 0)
	{
		LOG_ERR("Failed to set required pixel format\n");
		return;
	}

	if (cfb_framebuffer_init(dev))
	{
		LOG_ERR("Framebuffer initialization failed!\n");
		return;
	}

	cfb_framebuffer_clear(dev, true);

	display_blanking_off(dev);

  struct display_capabilities capabilities;
  display_get_capabilities(dev, &capabilities);

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

  cfb_framebuffer_clear(dev, false);
	cfb_framebuffer_set_font(dev, 0);

  if (display_set_contrast(dev, 0) != 0) {
    LOG_ERR("could not set display contrast");
  }
  size_t ms_sleep = 5;

  while (true) {
    // Increase brightness
    // for (size_t i = 0; i < 255; i++) {
    // 	display_set_contrast(dev, i);
		// 	cfb_framebuffer_clear(dev, false);
		// 	cfb_framebuffer_set_font(dev, 0);
		// 	snprintf(buf, sizeof(buf), "%d", i);
		// 	cfb_print(dev, buf, 0, 0);
		// 	cfb_framebuffer_finalize(dev);
    // 	k_sleep(K_MSEC(ms_sleep));
    // }

    // // Decrease brightness
    // for (size_t i = 255; i > 0; i--) {
    // 	display_set_contrast(dev, i);
		// 	cfb_framebuffer_clear(dev, false);
		// 	cfb_framebuffer_set_font(dev, 0);
		// 	snprintf(buf, sizeof(buf), "%d", i);
		// 	cfb_print(dev, buf, 0, 0);
		// 	cfb_framebuffer_finalize(dev);
    // 	k_sleep(K_MSEC(ms_sleep));
    // }
		// cfb_framebuffer_finalize(dev);


  
  }
}
