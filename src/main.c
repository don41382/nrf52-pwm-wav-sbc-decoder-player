
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>

#include <drivers/gpio.h>

#include <nrfx_pwm.h>
#include <hal/nrf_gpio.h>

#include "audio/buzzer02_16kHz.h"
#include "audio/buzzer01_16kHz.h"
#include "audio/decoder.h"

#include "nrf_pwm_audio.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define LED0        DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_NODE   DT_ALIAS(led0)
#define LED_PIN     DT_GPIO_PIN(LED0_NODE, gpios)
#define LED_FLAGS   DT_GPIO_FLAGS(LED0_NODE, gpios)

#define AUDIO_PIN NRF_GPIO_PIN_MAP(0,15)

void ledInit() {
	const struct device *dev = device_get_binding(LED0);	
	gpio_pin_configure(dev, LED_PIN, GPIO_OUTPUT_ACTIVE | LED_FLAGS);
}

void ledOn(bool on) {
	const struct device *dev = device_get_binding(LED0);
	gpio_pin_set(dev, LED_PIN, on);
}

void main(void) {

	LOG_INF("Playback time!");
	ledInit();
	
	IRQ_DIRECT_CONNECT(PWM0_IRQn, 0, nrfx_pwm_0_irq_handler, 0);
	nrf_pwm_audio_init(AUDIO_PIN, false);

	const size_t PCM_SIZE = 34688;
	unsigned char * pcmResult = (unsigned char *)k_malloc(PCM_SIZE);
    decodeSbcFile(&buzzer01_16kHz, buzzer01_16kHz_size, pcmResult);
	nrf_pwm_audio_playback(pcmResult, PCM_SIZE, NRF_PWM_AUDIO_SAMPLERATE_16K, 3.0, 0);

	ledOn(true);
	while (nrf_pwm_audio_is_playing()) { __WFE(); }
	ledOn(false);

	nrf_pwm_audio_stop();
	k_free(pcmResult);

  	while (1) {
		  k_msleep(2000);
	}
}