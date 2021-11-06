#pragma once

#include <zephyr.h>
#include <stdint.h>
#include "opus.h"
#include "nrftalk_opus.h"
#include "nrf.h"
#include "app_error.h"

#define OPUS_DECODER_SIZE 9224
#define AUDIO_SAMPLING_FREQUENCY 8000
#define AUDIO_CHANNEL 1

#define AUDIO_FRAME_SIZE_MS 5 // ms of each frame
#define AUDIO_BITRATE_LIMIT 11000 // bits per second
#define AUDIO_FRAME_SIZE_SAMPLES (AUDIO_FRAME_SIZE_MS * AUDIO_SAMPLING_FREQUENCY / 1000)

#define OPUS_AUDIO_FRAME_SIZE_BYTES ((AUDIO_BITRATE_LIMIT * AUDIO_FRAME_SIZE_SAMPLES / (8 * AUDIO_SAMPLING_FREQUENCY)))

#include <logging/log.h>

__ALIGN(4) static uint8_t           m_opus_decoder[OPUS_DECODER_SIZE];
static OpusDecoder * const          m_opus_state_decoder = (OpusDecoder *) m_opus_decoder;

typedef struct {
    uint16_t    data_size;
	uint8_t     data[OPUS_AUDIO_FRAME_SIZE_BYTES];
} opus_frame_t;

void opus_init() {
    size_t decoder_size = opus_decoder_get_size(AUDIO_CHANNEL);
    if (decoder_size != OPUS_DECODER_SIZE) {
        printk("error while checking decoder size. Please adjust to: %d\n", decoder_size);
    }
    int res = opus_decoder_init(m_opus_state_decoder, AUDIO_SAMPLING_FREQUENCY, AUDIO_CHANNEL);
    if (res != OPUS_OK) {
        printk("error while initializing opus decoder\n");
    } else {
        printk("opus decoder initialized\n");
    }
}

void play() {
    int16_t out[AUDIO_FRAME_SIZE_SAMPLES * AUDIO_CHANNEL * sizeof(uint16_t)];

    opus_frame_t frame;
    memcpy(frame.data, nrfistalking_opus_8kHz_32k, OPUS_AUDIO_FRAME_SIZE_BYTES);
    frame.data_size = OPUS_AUDIO_FRAME_SIZE_BYTES;

    drv_audio_codec_decode(&frame, out);

    int spf = opus_packet_get_samples_per_frame(nrfistalking_opus_8kHz_32k, AUDIO_SAMPLING_FREQUENCY);
    int bandwidth = opus_packet_get_bandwidth(nrfistalking_opus_8kHz_32k);
    int channels = opus_packet_get_nb_channels(nrfistalking_opus_8kHz_32k);
    printk("samples per frame: %d, bandwith: %d, channels: %d\n", spf, bandwidth, channels); // 160
}

void drv_audio_codec_decode(opus_frame_t *p_frame, int16_t *output_samples) {
    int ret;
    
    ret = opus_decode(
            m_opus_state_decoder,
            (p_frame->data_size == 0)? NULL :p_frame->data,
			p_frame->data_size,
			output_samples,
			AUDIO_FRAME_SIZE_SAMPLES,
			0);
   if(ret != AUDIO_FRAME_SIZE_SAMPLES){
	   printk("decode fail with error: %d!\n",ret);
   }

}
