#include <zephyr.h>
#include <device.h>
#include <devicetree.h>

#include <drivers/gpio.h>

#include <nrfx_pwm.h>
#include <hal/nrf_gpio.h>

#include "simple-pwm.h"
#include "nrftalk_raw.h"
#include "nrf_pwm_audio.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define LED0        DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_NODE   DT_ALIAS(led0)
#define LED_PIN     DT_GPIO_PIN(LED0_NODE, gpios)
#define LED_FLAGS   DT_GPIO_FLAGS(LED0_NODE, gpios)

#define HANK_PIN NRF_GPIO_PIN_MAP(0,13)

int count = 0;

void ledInit() {
	const struct device *dev = device_get_binding(LED0);	
	gpio_pin_configure(dev, LED_PIN, GPIO_OUTPUT_ACTIVE | LED_FLAGS);
}

void ledOn(bool on) {
	const struct device *dev = device_get_binding(LED0);
	gpio_pin_set(dev, LED_PIN, on);
}

void main(void) {

	LOG_INF("Playback from LOG :-)");
	ledInit();

	IRQ_DIRECT_CONNECT(PWM0_IRQn, 0, nrfx_pwm_0_irq_handler, 0);
	nrf_pwm_audio_init(HANK_PIN, false);

	ledOn(true);
	nrf_pwm_audio_playback(nrfistalking_raw_11kHz, nrfistalking_raw_11kHz_size, NRF_PWM_AUDIO_SAMPLERATE_16K, 1.0, 0);
	while (nrf_pwm_audio_is_playing()) { __WFE(); }
	LOG_INF("Done %d", count++);
	ledOn(false);

  	while (1) {
		  k_msleep(2000);
	}
}