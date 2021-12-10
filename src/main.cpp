#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <irq.h>
#include <nrf_erratas.h>

#include <drivers/gpio.h>
#include <nrfx_pwm.h>
#include <hal/nrf_gpio.h>

#include "audio/PWMPlayer.h"

#include "samples/hit.h"
#include "samples/itsworking.h"
#include "samples/liftup.h"

#include "audio/AudioDecoderSbc.h"
#include "audio/AudioDecoderRaw.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define AUDIO_PIN NRF_GPIO_PIN_MAP(0,13)

void play(PWMPlayer * player, const unsigned char * data, size_t size) {
	AudioDecoderSbc sbcDecoder(data, size);
	player->play(&sbcDecoder, AudioSampleRate::SAMPLERATE_16K);
	while(player->isPlaying()) { __WFE(); }
}

extern "C" void main(void) {
	LOG_INF("Playback time!!");
	IRQ_DIRECT_CONNECT(PWM0_IRQn, 0, nrfx_pwm_0_irq_handler, 0);

	PWMPlayer player;
	player.init(AUDIO_PIN);

	play(&player, hit, hit_size);
	play(&player, itsworking, itsworking_size);
	player.destory();
	
  	while (1) {
		  k_msleep(10000);
	}
}
