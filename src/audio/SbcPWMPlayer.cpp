#include <zephyr.h>
#include <nrfx.h>

#include "SbcPWMPlayer.h"

void SbcPWMPlayer::pwm_buffer_filler(nrfx_pwm_evt_type_t event_type, void * p_context) {
    SbcPWMPlayer * player = (SbcPWMPlayer *)p_context;
    switch (event_type) {
        case NRFX_PWM_EVT_END_SEQ0:
            player->fillSequenceBuffer(0);
            break;
        case NRFX_PWM_EVT_END_SEQ1:
            player->fillSequenceBuffer(1);
            break;
        default:
            break;
    }
}

nrfx_err_t SbcPWMPlayer::init(uint8_t pwmPin) {

    nrfx_pwm_config_t pwm_config= {
        .output_pins  = { pwmPin,
                            NRFX_PWM_PIN_NOT_USED,
                            NRFX_PWM_PIN_NOT_USED,
                            NRFX_PWM_PIN_NOT_USED },
        .irq_priority = NRF_PWM_AUDIO_PRIORITY,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = NRF_PWM_AUDIO_COUNTERTOP,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO      
    };

    nrfx_err_t retCode = nrfx_pwm_init(&m_pwm, &pwm_config, pwm_buffer_filler, this);
    if (NRFX_SUCCESS != retCode) { return retCode; }
    this->initSequences();

    return NRFX_SUCCESS;
}

void SbcPWMPlayer::initSequences() {
    for (int i=0; i<2; i++) {
        pwmSeqs[i].values.p_raw = pwmSeqBuffer[i];
        pwmSeqs[i].length= NRF_PWM_AUDIO_BUFFER_LENGTH;
        pwmSeqs[i].repeats= 0;
        pwmSeqs[i].end_delay= 0;
    }
}

uint16_t pcm2pwm(unsigned char pcm, float gain) {
    float sample= ((float) (int8_t)pcm) * (gain / 128.0f);
    
    if ((sample > -1.0f) && (sample < 1.0f)) {
        return (uint16_t) ((sample+1.0f) * ((float) NRF_PWM_AUDIO_COUNTERTOP) / 2.0f);
    } else if (sample == 0) {
        return NRF_PWM_AUDIO_COUNTERTOP / 2;
    } else if (sample <= -1.0f) {
        return 0;
    } else if (sample >= 1.0f) {
        return NRF_PWM_AUDIO_COUNTERTOP;
    } else {
        return 0;
    }
}

void SbcPWMPlayer::fillSequenceBuffer(uint8_t sequenceId) {
    int srf = sampleRate;
    int readSize = decoder->decode(decoderBuffer, NRF_PWM_AUDIO_BUFFER_LENGTH / srf);
    
    if (readSize > 0) {
        for (int i=0; i<NRF_PWM_AUDIO_BUFFER_LENGTH; ++i) {
            if ((i/srf) < readSize) {
                pwmSeqBuffer[sequenceId][i] = pcm2pwm(decoderBuffer[i / srf], 4.0);
            } else {
                pwmSeqBuffer[sequenceId][i] = NRF_PWM_AUDIO_COUNTERTOP / 2;
            }
        }
    } else {
        for (int i=0; i<NRF_PWM_AUDIO_BUFFER_LENGTH; i++) {
            pwmSeqBuffer[sequenceId][i] = NRF_PWM_AUDIO_COUNTERTOP / 2;
        }
        nrfx_pwm_stop(&m_pwm, true);
    }
}

void SbcPWMPlayer::play(AudioDecoder * decoder, AudioSampleRate rate) {
    this->decoder = decoder;
    this->sampleRate = rate;
    decoder->reset();

    nrfx_pwm_complex_playback(&m_pwm, &pwmSeqs[0], &pwmSeqs[1], 1, NRFX_PWM_FLAG_SIGNAL_END_SEQ0 | NRFX_PWM_FLAG_SIGNAL_END_SEQ1 | NRFX_PWM_FLAG_LOOP);
}

void SbcPWMPlayer::stop() {
    playing = false;
}

bool SbcPWMPlayer::isPlaying() {
    return !nrfx_pwm_is_stopped(&m_pwm);
}

void SbcPWMPlayer::destory() {
    
}