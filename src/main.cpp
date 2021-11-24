#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <irq.h>
#include <nrf_erratas.h>

#include <drivers/gpio.h>
#include <nrfx_pwm.h>
#include <hal/nrf_gpio.h>

#include "audio/decoder.h"
#include "audio/buzzer03_16kHz.h"
#include "audio/buzzer04_16kHz.h"
#include "audio/SbcPWMPlayer.h"

#include "audio/AudioDecoderSbc.h"
#include "audio/AudioDecoderRaw.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define LED0        DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_NODE   DT_ALIAS(led0)
#define LED_PIN     DT_GPIO_PIN(LED0_NODE, gpios)
#define LED_FLAGS   DT_GPIO_FLAGS(LED0_NODE, gpios)

#define AUDIO_PIN NRF_GPIO_PIN_MAP(0,15)

void ledInit() {
	const struct device *dev = device_get_binding(LED0);	
	gpio_pin_configure(dev, LED_PIN, GPIO_OUTPUT | LED_FLAGS);
}

void ledOn(bool on) {
	const struct device *dev = device_get_binding(LED0);
	gpio_pin_set(dev, LED_PIN, on);
}

extern "C" void main(void) {
	LOG_INF("Playback time!");
	ledInit();

	IRQ_DIRECT_CONNECT(PWM0_IRQn, 0, nrfx_pwm_0_irq_handler, 0);
	
	SbcPWMPlayer player;
	AudioDecoderSbc sbcDecoder(buzzer04_16kHz, buzzer04_16kHz_size);

	player.init(AUDIO_PIN);
	player.play(&sbcDecoder, AudioSampleRate::SAMPLERATE_16K);
	
	while(player.isPlaying()) { __WFE(); }
	ledOn(false);


  	while (1) {
		  k_msleep(10000);
	}
}
