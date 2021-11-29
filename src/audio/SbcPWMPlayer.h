#pragma once

#include <stdint.h>
#include <stdio.h>
#include <functional>
#include <sbc.h>
#include "AudioDecoder.h"

#include <nrfx_pwm.h>

#define NRF_PWM_AUDIO_PWM_INSTANCE 0
#define NRF_PWM_AUDIO_PRIORITY 6
#define NRF_PWM_AUDIO_COUNTERTOP 512
#define NRF_PWM_AUDIO_BUFFER_LENGTH 512
#define NRF_PWM_AUDIO_SBC_BUFFER 512
#define NRF_PWM_SBC_PCM_BUFFER_LENGTH 512

typedef enum
{
  SAMPLERATE_31K = 1,           ///< use 31250 Hz sampling rate
  SAMPLERATE_16K = 2,           ///< use 15625 Hz sampling rate
  SAMPLERATE_8K  = 4            ///< use 7812.5 Hz sampling rate
} AudioSampleRate;

using namespace std;

class SbcPWMPlayer {
    private:
        AudioSampleRate sampleRate;
        
        AudioDecoder * decoder;
        unsigned char decoderBuffer[NRF_PWM_SBC_PCM_BUFFER_LENGTH] = {0};

        nrfx_pwm_t m_pwm= NRFX_PWM_INSTANCE(NRF_PWM_AUDIO_PWM_INSTANCE);
        nrf_pwm_sequence_t pwmSeqs[2]= {0}; 
        uint16_t pwmSeqBuffer[2][NRF_PWM_AUDIO_BUFFER_LENGTH]= {0};

        bool playing = false;
        void initSequences();
    public:
        nrfx_err_t init(uint8_t pwmPin);
        void play(AudioDecoder * decoder, AudioSampleRate sampleRate);
        bool isPlaying();

        void stop();
        static void pwm_buffer_filler(nrfx_pwm_evt_type_t event_type, void * p_context);

        void fillSequenceBuffer(uint8_t sequenceId);
        void destory();
};

