/**
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  @file     nrf_pwm_audio.c
 *  @author   Tamas Harczos
 *  @since    2019-10-01
 *  @version  0.1.0
 *  @licence  GNU LGPL v3 (https://www.gnu.org/licenses/lgpl-3.0.txt)
 * 
 *  @brief    PWM audio functionality on nRF52.
 *  
 *  Project homepage:  https://nrf52-pwm-audio.sourceforge.io/
 *  Youtube video:     https://youtu.be/_m4-pH_Yw3M
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
**/


// *******************************************************************************************************************
// Included Files (from specific to general)
// *******************************************************************************************************************

#include "nrf_pwm_audio.h"
#include "opusfile.h"
#include <hal/nrf_gpio.h>


// *******************************************************************************************************************
// Definitions
// *******************************************************************************************************************

bool static m_pwm_initialized= false;                                       ///< gets true during successful init(), becomes false during destroy()
bool static m_pwm_playback_in_progress= false;                              ///< every SPI transfers sets this flag and the 'SPI finished' callback clears it

nrf_pwm_sequence_t static m_pwm_seqs[2]= {0};                               ///< allocate memory for two PWM sequence structures
uint16_t static m_seq_buf[2][NRF_PWM_AUDIO_BUFFER_LENGTH]= {0};             ///< allocate memory for two sequence buffers

nrfx_pwm_t static m_pwm= NRFX_PWM_INSTANCE(NRF_PWM_AUDIO_PWM_INSTANCE);     ///< instantiate one PWM peripheral

#define NRF_PWM_AUDIO_COUNTERTOP 512                                        ///< this implies the supported sampling rates and gains; I suggest not to change it unless you know exactly what you are doing :)

nrf_pwm_audio_samplerate_t static m_upsample= NRF_PWM_AUDIO_SAMPLERATE_31K; ///< sampling rate
float static m_gain= 1.0f;                                                  ///< gain to be applied at playback

size_t static m_num_repeat= 0;                                              ///< number of repeats (after the first playback) requested
size_t static m_num_repeat_left= 0;                                         ///< number of repeats left

int8_t static const *m_pcm_data= NULL;                                      ///< pointer to the PCM values
size_t static m_pcm_data_length= 0;                                         ///< number of elements at the PCM data pointer
size_t static m_pcm_data_read_idx= 0;                                       ///< current PCM buffer read index 
size_t static m_num_sample_left= 0;                                         ///< PCM values left to be processed



// *******************************************************************************************************************
// Module internal functions
// *******************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Convert a buffer of PCM values into a PWM sequence.
///
/// @param[out]  pwm_seq_buffer  Pointer to the 16-bit unsigned buffer of the PWM sequence items.
/// @param[in]   pcm_buffer      Pointer to the 8-bit signed integer PCM values to be converted.
/// @param[in]   pcm_buf_length  Length of pcm_buffer.
/// @param[in    samplerate      Sample rate of the PCM data (8 kHz, 16 kHz, or 31 kHz).
/// @param[in]   scaler          Gain to be applied (2.0 ~= +6 dB).
///
/// @return  Number of bytes consumed from the pcm_buffer.
///
/// @note  pwm_seq_buffer must be able to hold enough data (in worst case pcm_buf_length*2 bytes).
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t static pcm_to_pwm(uint16_t *pwm_seq_buffer, int8_t const *pcm_buffer, size_t pcm_buf_length, nrf_pwm_audio_samplerate_t samplerate, float scaler)
{
  size_t srf= (size_t) samplerate;
  float sample;

  for (size_t i=0; i<NRF_PWM_AUDIO_BUFFER_LENGTH; ++i)
  {
    if (i/srf < pcm_buf_length)
    {
      sample= ((float) pcm_buffer[i/srf]) * (scaler / 128.0f);
    }
    else
    {
      sample= 0;
    }

    if ((sample > -1.0f) && (sample < 1.0f))
    {
      pwm_seq_buffer[i]= (uint16_t) ((sample+1.0f) * ((float) NRF_PWM_AUDIO_COUNTERTOP) / 2.0f);
    }
    else if (sample == 0)
    {
      pwm_seq_buffer[i]= NRF_PWM_AUDIO_COUNTERTOP / 2;
    }
    else if (sample <= -1.0f)
    {
      pwm_seq_buffer[i]= 0;
    }
    else if (sample >= 1.0f)
    {
      pwm_seq_buffer[i]= NRF_PWM_AUDIO_COUNTERTOP;
    }
  }

  return (NRF_PWM_AUDIO_BUFFER_LENGTH/srf);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Event handler of the PWM periphery. Handles filling of the PWM buffers.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void pwm_handler(nrfx_pwm_evt_type_t event_type, void * p_context)
{
  size_t i, consumed_pcm_samples= 0;

  switch (event_type)
  {
    case NRFX_PWM_EVT_END_SEQ0:

      if (m_pwm_playback_in_progress)
      {
        consumed_pcm_samples= pcm_to_pwm(m_seq_buf[0], &m_pcm_data[m_pcm_data_read_idx], m_num_sample_left, m_upsample, m_gain);
      }
      else
      {
        for (i=0; i<NRF_PWM_AUDIO_BUFFER_LENGTH; ++i) { m_seq_buf[0][i]= NRF_PWM_AUDIO_COUNTERTOP / 2; }
      }

    break;

    case NRFX_PWM_EVT_END_SEQ1:

      if (m_pwm_playback_in_progress)
      {
        consumed_pcm_samples= pcm_to_pwm(m_seq_buf[1], &m_pcm_data[m_pcm_data_read_idx], m_num_sample_left, m_upsample, m_gain);
      }
      else
      {
        for (i=0; i<NRF_PWM_AUDIO_BUFFER_LENGTH; ++i) { m_seq_buf[1][i]= NRF_PWM_AUDIO_COUNTERTOP / 2; }
      }

    break;

    default:
    return;
  }

  m_pcm_data_read_idx += consumed_pcm_samples;
  m_num_sample_left -= consumed_pcm_samples;

  if (m_pcm_data_read_idx >= m_pcm_data_length)
  {
    if (m_num_repeat_left > 0)
    {
      m_num_repeat_left--;
      m_num_sample_left = m_pcm_data_length;
      m_pcm_data_read_idx= 0;
    }
    else
    {
      m_num_sample_left= 0;
      m_pwm_playback_in_progress= false;
    }
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Fills both PWM sequence buffers with neutral values.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void clear_buffers(void)
{
  for (size_t i=0; i<NRF_PWM_AUDIO_BUFFER_LENGTH; ++i)
  {
    m_seq_buf[0][i]= m_seq_buf[1][i]= NRF_PWM_AUDIO_COUNTERTOP / 2;
  }
}


// *******************************************************************************************************************
// Public functions
// *******************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nrfx_err_t nrf_pwm_audio_init(uint8_t gpio_pin, bool high_drive)
{
  nrfx_err_t retCode;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // check if PWM was already initialized (yes -> this function was not called for the first time)
  if (m_pwm_initialized)
  {
    nrf_pwm_audio_destroy();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  nrfx_pwm_config_t const pwm_config= 
  {
    .output_pins  = { gpio_pin | NRFX_PWM_PIN_INVERTED,
                      NRFX_PWM_PIN_NOT_USED,
                      NRFX_PWM_PIN_NOT_USED,
                      NRFX_PWM_PIN_NOT_USED },
    .irq_priority = NRF_PWM_AUDIO_PRIORITY,
    .base_clock   = NRF_PWM_CLK_16MHz,
    .count_mode   = NRF_PWM_MODE_UP,
    .top_value    = NRF_PWM_AUDIO_COUNTERTOP,
    .load_mode    = PWM_DECODER_LOAD_Common,
    .step_mode    = PWM_DECODER_MODE_RefreshCount      
  };

  retCode= nrfx_pwm_init(&m_pwm, &pwm_config, pwm_handler, NULL);
  if (NRFX_SUCCESS != retCode) { return retCode; }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  clear_buffers();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  if (high_drive)
  {
    nrf_gpio_cfg(gpio_pin, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_H0S1, NRF_GPIO_PIN_NOSENSE);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  m_pwm_seqs[0].values= ((nrf_pwm_values_t) ((uint16_t const*) m_seq_buf[0]));
  m_pwm_seqs[0].length= NRF_PWM_AUDIO_BUFFER_LENGTH;
  m_pwm_seqs[0].repeats= 0;
  m_pwm_seqs[0].end_delay= 0;

  m_pwm_seqs[1].values= ((nrf_pwm_values_t) ((uint16_t const*) m_seq_buf[1]));
  m_pwm_seqs[1].length= NRF_PWM_AUDIO_BUFFER_LENGTH;
  m_pwm_seqs[1].repeats= 0;
  m_pwm_seqs[1].end_delay= 0;

  m_pwm_initialized= true;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  return NRFX_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void nrf_pwm_audio_destroy(void)
{
  if (m_pwm_initialized)
  {
    nrf_pwm_audio_stop();
    nrfx_pwm_uninit(&m_pwm);
  }

  m_pwm_playback_in_progress= false;
  m_pwm_initialized= false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nrfx_err_t nrf_pwm_audio_playback(uint8_t const *pcm_data, size_t pcm_data_length, nrf_pwm_audio_samplerate_t samplerate, float gain, size_t num_repeat)
{
  if (!m_pwm_initialized) { return NRFX_ERROR_INTERNAL; }

  m_pcm_data= pcm_data;
  m_pcm_data_length= pcm_data_length;
  m_upsample= samplerate;
  m_gain= gain;
  m_num_repeat= num_repeat;

  m_pcm_data_read_idx= 0;
  m_num_repeat_left= num_repeat;
  m_num_sample_left= pcm_data_length;

  m_pwm_playback_in_progress= true;

  nrfx_pwm_complex_playback(&m_pwm, &m_pwm_seqs[0], &m_pwm_seqs[1], 1, NRFX_PWM_FLAG_SIGNAL_END_SEQ0 | NRFX_PWM_FLAG_SIGNAL_END_SEQ1 | NRFX_PWM_FLAG_LOOP);

  return NRFX_SUCCESS;
}

nrfx_err_t nrf_pwm_audio_playback_opus(uint8_t const *opus_data, size_t opus_data_length, nrf_pwm_audio_samplerate_t samplerate, float gain, size_t num_repeat)
{
  if (!m_pwm_initialized) { return NRFX_ERROR_INTERNAL; }

  int err;
  OggOpusFile * oggFile = op_open_memory(opus_data, opus_data_length, &err);
    if (err == 0) {
        printk("open opus file worked!\n");
    } else {
        printk("error during opus file opening: %d\n", err);
    }

  //m_pcm_data= pcm_data;
  //m_pcm_data_length= pcm_data_length;
  m_upsample= samplerate;
  m_gain= gain;
  m_num_repeat= num_repeat;

  m_pcm_data_read_idx= 0;
  m_num_repeat_left= num_repeat;
  //m_num_sample_left= pcm_data_length;

  m_pwm_playback_in_progress= true;

  nrfx_pwm_complex_playback(&m_pwm, &m_pwm_seqs[0], &m_pwm_seqs[1], 1, NRFX_PWM_FLAG_SIGNAL_END_SEQ0 | NRFX_PWM_FLAG_SIGNAL_END_SEQ1 | NRFX_PWM_FLAG_LOOP);

  return NRFX_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nrfx_err_t nrf_pwm_audio_stop(void)
{
  if (!m_pwm_initialized) { return NRFX_ERROR_INTERNAL; }

  nrfx_pwm_stop(&m_pwm, false);

  clear_buffers();

  return NRFX_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t nrf_pwm_audio_repeats_left(void)
{
  return (m_pwm_initialized)?(m_num_repeat_left):(0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t nrf_pwm_audio_samples_left(void)
{
  return (m_pwm_initialized)?(m_num_sample_left):(0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool nrf_pwm_audio_is_playing(void)
{
  return (m_pwm_initialized)?(m_pwm_playback_in_progress):(false);
}



// *******************************************************************************************************************
// End of File
// *******************************************************************************************************************
