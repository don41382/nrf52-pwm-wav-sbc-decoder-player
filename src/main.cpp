#include <zephyr.h>

#include <hal/nrf_gpio.h>

#include "audio/PWMPlayer.h"
#include "audio/AudioDecoderSbc.h"
#include "audio/AudioDecoderRaw.h"

#include "samples/sample_hit_raw.h"
#include "samples/sample_itsworking_sbc.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#define AUDIO_PIN NRF_GPIO_PIN_MAP(0,13)

void playRaw(PWMPlayer * player, const unsigned char * data, size_t size) {
	AudioDecoderRaw decoder(data, size, SAMPLERATE_16K);
	player->play(&decoder, 2.0);
	while(player->isPlaying()) { __WFE(); }
}

void playSbc(PWMPlayer * player, const unsigned char * data, size_t size) {
	AudioDecoderSbc decoder(data, size);
	player->play(&decoder, 2.0);
	while(player->isPlaying()) { __WFE(); }
}


extern "C" void main(void) {
	LOG_INF("Playback time!!");
	IRQ_DIRECT_CONNECT(PWM0_IRQn, 0, nrfx_pwm_0_irq_handler, 0);

	PWMPlayer player;
	player.init(AUDIO_PIN);

	playRaw(&player, sample_hit_raw, sample_hit_raw_size);
	playSbc(&player, sample_itsworking_sbc, sample_itsworking_sbc_size);
	player.destory();
	
  	while (1) {
		  k_msleep(10000);
	}
}
