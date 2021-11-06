/**
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  @file     nrf_pwm_audio.h
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


#ifndef _NRF_PWM_AUDIO_H
#define _NRF_PWM_AUDIO_H



// *******************************************************************************************************************
// Included Files (from specific to general)
// *******************************************************************************************************************

#include "nrfx_pwm.h"

#include <stdbool.h>
#include <stdint.h>



// *******************************************************************************************************************
// Provide C++ Compatibility
// *******************************************************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif

               
               
// *******************************************************************************************************************
// Global declarations
// *******************************************************************************************************************

#define NRF_PWM_AUDIO_PWM_INSTANCE  0         ///< the number of PWM instance to use
#define NRF_PWM_AUDIO_BUFFER_LENGTH 512       ///< length of each of the two PWM sequence buffers
#define NRF_PWM_AUDIO_PRIORITY      6         ///< requested priority of the PWM peripheral


typedef enum
{
  NRF_PWM_AUDIO_SAMPLERATE_31K = 1,           ///< use 31250 Hz sampling rate
  NRF_PWM_AUDIO_SAMPLERATE_16K = 2,           ///< use 15625 Hz sampling rate
  NRF_PWM_AUDIO_SAMPLERATE_8K  = 4            ///< use 7812.5 Hz sampling rate
} nrf_pwm_audio_samplerate_t;


// *******************************************************************************************************************
// Public functions
// *******************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Initialize the PWM periphery and clear buffers.
///
/// @param[in]  gpio_pin    Number of the GPIO pin to be used as output.
/// @param[in]  high_drive  Whether to enable higher output drive strength for more loudness.
///
/// @return  NRFX_SUCCESS on success or an NRFX_ERROR_* code (as listed in nrfx_glue.h) upon failure.
///
/// @note  This function should be called before calling any other function from this module.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nrfx_err_t nrf_pwm_audio_init(uint8_t gpio_pin, bool high_drive);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Stops any running PWM playback and uninitializes the PWM periphery.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void nrf_pwm_audio_destroy(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Initiates a playback of the given audio clip.
///
/// @param[in]  pcm_data         Pointer to the 8-bit signed integer PCM audio data.
/// @param[in]  pcm_data_length  Number of samples to be played back.
/// @param[in]  samplerate       Desired sample rate for the playback (8 kHz, 16 kHz, or 31 kHz).
/// @param[in]  gain             Gain to be applied before playback (2.0 ~= +6 dB).
/// @param[in]  num_repeat       Number of repeats after the first playback.
///
/// @return  NRFX_SUCCESS on success or NRFX_ERROR_INTERNAL if the module has not been initialized previously.
///
/// @note  With the default PWM clock (16 MHz) and COUNTERTOP setting (512) it is safe to apply a gain <= 2.0 without
///        the risk of distortion.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nrfx_err_t nrf_pwm_audio_playback(uint8_t const *pcm_data, size_t pcm_data_length, nrf_pwm_audio_samplerate_t samplerate, float gain, size_t num_repeat);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Stops any running playback, clears the buffers, and stops the PWM periphery (but leaves it initialized).
///
/// @return  NRFX_SUCCESS on success or NRFX_ERROR_INTERNAL if the module has not been initialized previously.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
nrfx_err_t nrf_pwm_audio_stop(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Returns the number of repeats left. If the module has not yet been initialized, the function returns 0.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t nrf_pwm_audio_repeats_left(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Returns the number of samples still to be played back in the current loop. To calculate the total number
///         of samples still to be played back, use the following formula: 
///         pcm_data_length * nrf_pwm_audio_repeats_left() + nrf_pwm_audio_samples_left().
///         If the module has not yet been initialized, the function returns 0.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t nrf_pwm_audio_samples_left(void);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Return true if audio playback is currently active. If the module has not yet been initialized, the
///         function returns false.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool nrf_pwm_audio_is_playing(void);



  
// *******************************************************************************************************************
// Provide C++ Compatibility
// *******************************************************************************************************************
#ifdef __cplusplus
}
#endif


#endif // _NRF_PWM_AUDIO_H



// *******************************************************************************************************************
// End of File
// *******************************************************************************************************************
