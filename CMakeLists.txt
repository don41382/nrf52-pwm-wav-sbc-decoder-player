# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf52-pwm-with-opus-player)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
 -DARM_MATH_CM4 \
 -DHAVE_CONFIG_H \
 -DDISABLE_FLOAT_API \
 -DFIXED_POINT \
 -DOP_FIXED_POINT \
 -DDHAVE_LRINT \
 -DHAVE_LRINTF \
 -DFLOAT_ABI_HARD \
 -DOPUS_ARM_ASM \
 -DOPUS_ARM_INLINE_ASM \
 -DOPUS_ARM_INLINE_EDSP \
 -DOPUS_ARM_INLINE_MEDIA \
 -DOPUS_ARM_MAY_HAVE_EDSP \
 -DOPUS_ARM_PRESUME_EDSP \
 -DOPUS_BUILD \
 -DVAR_ARRAYS \
 -g \
")

target_sources(app PRIVATE
  lib/opus-1.2.1/A2NLSF.c 
  lib/opus-1.2.1/CNG.c 
  lib/opus-1.2.1/HP_variable_cutoff.c 
  lib/opus-1.2.1/LPC_analysis_filter.c 
  lib/opus-1.2.1/LPC_fit.c 
  lib/opus-1.2.1/LPC_inv_pred_gain.c 
  lib/opus-1.2.1/LP_variable_cutoff.c 
  lib/opus-1.2.1/LTP_analysis_filter_FIX.c 
  lib/opus-1.2.1/LTP_scale_ctrl_FIX.c 
  lib/opus-1.2.1/NLSF2A.c 
  lib/opus-1.2.1/NLSF_VQ.c 
  lib/opus-1.2.1/NLSF_VQ_weights_laroia.c 
  lib/opus-1.2.1/NLSF_decode.c 
  lib/opus-1.2.1/NLSF_del_dec_quant.c 
  lib/opus-1.2.1/NLSF_encode.c 
  lib/opus-1.2.1/NLSF_stabilize.c 
  lib/opus-1.2.1/NLSF_unpack.c 
  lib/opus-1.2.1/NSQ.c 
  lib/opus-1.2.1/NSQ_del_dec.c 
  lib/opus-1.2.1/PLC.c 
  lib/opus-1.2.1/VAD.c 
  lib/opus-1.2.1/VQ_WMat_EC.c 
  lib/opus-1.2.1/ana_filt_bank_1.c 
  lib/opus-1.2.1/analysis.c 
  lib/opus-1.2.1/apply_sine_window_FIX.c 
  lib/opus-1.2.1/autocorr_FIX.c 
  lib/opus-1.2.1/bands.c 
  lib/opus-1.2.1/biquad_alt.c 
  lib/opus-1.2.1/burg_modified_FIX.c 
  lib/opus-1.2.1/bwexpander.c 
  lib/opus-1.2.1/bwexpander_32.c 
  lib/opus-1.2.1/celt.c 
  lib/opus-1.2.1/celt_decoder.c 
  lib/opus-1.2.1/celt_encoder.c 
  lib/opus-1.2.1/celt_lpc.c 
  lib/opus-1.2.1/arm/celt_pitch_xcorr_arm_gcc.s 
  lib/opus-1.2.1/check_control_input.c 
  lib/opus-1.2.1/code_signs.c 
  lib/opus-1.2.1/control_SNR.c 
  lib/opus-1.2.1/control_audio_bandwidth.c 
  lib/opus-1.2.1/control_codec.c 
  lib/opus-1.2.1/corrMatrix_FIX.c 
  lib/opus-1.2.1/cwrs.c 
  lib/opus-1.2.1/debug.c 
  lib/opus-1.2.1/dec_API.c 
  lib/opus-1.2.1/decode_core.c 
  lib/opus-1.2.1/decode_frame.c 
  lib/opus-1.2.1/decode_indices.c 
  lib/opus-1.2.1/decode_parameters.c 
  lib/opus-1.2.1/decode_pitch.c 
  lib/opus-1.2.1/decode_pulses.c 
  lib/opus-1.2.1/decoder_set_fs.c 
  lib/opus-1.2.1/enc_API.c 
  lib/opus-1.2.1/encode_frame_FIX.c 
  lib/opus-1.2.1/encode_indices.c 
  lib/opus-1.2.1/encode_pulses.c 
  lib/opus-1.2.1/entcode.c 
  lib/opus-1.2.1/entdec.c 
  lib/opus-1.2.1/entenc.c 
  lib/opus-1.2.1/find_LPC_FIX.c 
  lib/opus-1.2.1/find_LTP_FIX.c 
  lib/opus-1.2.1/find_pitch_lags_FIX.c 
  lib/opus-1.2.1/find_pred_coefs_FIX.c 
  lib/opus-1.2.1/gain_quant.c 
  lib/opus-1.2.1/init_decoder.c 
  lib/opus-1.2.1/init_encoder.c 
  lib/opus-1.2.1/inner_prod_aligned.c 
  lib/opus-1.2.1/interpolate.c 
  lib/opus-1.2.1/k2a_FIX.c 
  lib/opus-1.2.1/k2a_Q16_FIX.c 
  lib/opus-1.2.1/kiss_fft.c 
  lib/opus-1.2.1/laplace.c 
  lib/opus-1.2.1/lin2log.c 
  lib/opus-1.2.1/log2lin.c 
  lib/opus-1.2.1/mathops.c 
  lib/opus-1.2.1/mdct.c 
  lib/opus-1.2.1/mlp.c 
  lib/opus-1.2.1/mlp_data.c 
  lib/opus-1.2.1/modes.c 
  lib/opus-1.2.1/noise_shape_analysis_FIX.c 
  lib/opus-1.2.1/opus.c 
  lib/opus-1.2.1/opus_decoder.c 
  lib/opus-1.2.1/opus_encoder.c 
  lib/opus-1.2.1/opus_multistream.c 
  lib/opus-1.2.1/opus_multistream_decoder.c 
  lib/opus-1.2.1/opus_multistream_encoder.c 
  
  lib/opus-1.2.1/pitch.c 
  lib/opus-1.2.1/pitch_analysis_core_FIX.c 
  lib/opus-1.2.1/pitch_est_tables.c 
  lib/opus-1.2.1/process_NLSFs.c 
  lib/opus-1.2.1/process_gains_FIX.c 
  lib/opus-1.2.1/quant_LTP_gains.c 
  lib/opus-1.2.1/quant_bands.c 
  lib/opus-1.2.1/rate.c 
  lib/opus-1.2.1/regularize_correlations_FIX.c 
  lib/opus-1.2.1/repacketizer.c 
  lib/opus-1.2.1/resampler.c 
  lib/opus-1.2.1/resampler_down2.c 
  lib/opus-1.2.1/resampler_down2_3.c 
  lib/opus-1.2.1/resampler_private_AR2.c 
  lib/opus-1.2.1/resampler_private_IIR_FIR.c 
  lib/opus-1.2.1/resampler_private_down_FIR.c 
  lib/opus-1.2.1/resampler_private_up2_HQ.c 
  lib/opus-1.2.1/resampler_rom.c 
  lib/opus-1.2.1/residual_energy16_FIX.c 
  lib/opus-1.2.1/residual_energy_FIX.c 
  lib/opus-1.2.1/schur64_FIX.c 
  lib/opus-1.2.1/schur_FIX.c 
  lib/opus-1.2.1/shell_coder.c 
  lib/opus-1.2.1/sigm_Q15.c 
  lib/opus-1.2.1/sort.c 
  lib/opus-1.2.1/stereo_LR_to_MS.c 
  lib/opus-1.2.1/stereo_MS_to_LR.c 
  lib/opus-1.2.1/stereo_decode_pred.c 
  lib/opus-1.2.1/stereo_encode_pred.c 
  lib/opus-1.2.1/stereo_find_predictor.c 
  lib/opus-1.2.1/stereo_quant_pred.c 
  lib/opus-1.2.1/sum_sqr_shift.c 
  lib/opus-1.2.1/table_LSF_cos.c 
  lib/opus-1.2.1/tables_LTP.c 
  lib/opus-1.2.1/tables_NLSF_CB_NB_MB.c 
  lib/opus-1.2.1/tables_NLSF_CB_WB.c 
  lib/opus-1.2.1/tables_gain.c 
  lib/opus-1.2.1/tables_other.c 
  lib/opus-1.2.1/tables_pitch_lag.c 
  lib/opus-1.2.1/tables_pulses_per_block.c 
  lib/opus-1.2.1/vector_ops_FIX.c 
  lib/opus-1.2.1/vq.c 
  lib/opus-1.2.1/warped_autocorrelation_FIX.c 

  lib/opusfile-0.12/src/info.c
  lib/opusfile-0.12/src/internal.c
  lib/opusfile-0.12/src/opusfile.c
  lib/opusfile-0.12/src/stream.c
  lib/libogg-1.3.5/src/framing.c

  src/ogg/k_realloc.c
  src/nrf_pwm_audio.c
  src/main.c
)

zephyr_include_directories(
  src
  src/config
  lib/opus-1.2.1
  lib/opusfile-0.12/include
  lib/libogg-1.3.5/include
)
