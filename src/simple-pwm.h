#pragma once

#include <nrfx_pwm.h>
#include <devicetree.h>
#include <device.h>

void pwmHandler(nrfx_pwm_evt_type_t event_type, void * p_context) {
	printk("handler!\n");
}

void complexPwmExample(uint8_t pin) {
	static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

	uint32_t err_code;
	nrfx_pwm_config_t const config0 = {
		.output_pins =
		{
			pin | NRFX_PWM_PIN_INVERTED,
			NRFX_PWM_PIN_NOT_USED,
			NRFX_PWM_PIN_NOT_USED,
			NRFX_PWM_PIN_NOT_USED,
		},
		.irq_priority = 7,
		.base_clock   = NRF_PWM_CLK_125kHz,
		.count_mode   = NRF_PWM_MODE_UP,
		.top_value    = 1000,
		.load_mode    = NRF_PWM_LOAD_COMMON,
		.step_mode    = NRF_PWM_STEP_AUTO
	};
	err_code = nrfx_pwm_init(&m_pwm0, &config0, pwmHandler, NULL);
	if (!err_code)
	{
		printk("NRF PWM could not be initialized: %d\n", err_code);
	}

	static nrf_pwm_values_common_t seq_values[] = {
    	0, 1000
	};

	nrf_pwm_sequence_t const seq = {
    	.values.p_common = seq_values,
    	.length          = NRF_PWM_VALUES_LENGTH(seq_values),
    	.repeats         = 0,
    	.end_delay       = 0
	};

	nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
}

void simplePwmExample(uint8_t pin) {
    static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

  	// see https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.0.0%2Fhardware_driver_pwm.html
    // low power PWM for LED blinking
	
	uint32_t err_code;
	nrfx_pwm_config_t const config0 = {
		.output_pins =
		{
			pin | NRFX_PWM_PIN_INVERTED, // channel 0
			NRFX_PWM_PIN_NOT_USED,             // channel 1
			NRFX_PWM_PIN_NOT_USED,             // channel 2
			NRFX_PWM_PIN_NOT_USED,             // channel 3
		},
		.irq_priority = 7,
		.base_clock   = NRF_PWM_CLK_125kHz,
		.count_mode   = NRF_PWM_MODE_UP,
		.top_value    = 1000,
		.load_mode    = NRF_PWM_LOAD_COMMON,
		.step_mode    = NRF_PWM_STEP_AUTO
	};
	err_code = nrfx_pwm_init(&m_pwm0, &config0, NULL, NULL);
	if (!err_code)
	{
		printk("NRF PWM could not be initialized: %d\n", err_code);
	}

	static nrf_pwm_values_common_t seq_values[] = {
    	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1000
	};

	nrf_pwm_sequence_t const seq = {
    	.values.p_common = seq_values,
    	.length          = NRF_PWM_VALUES_LENGTH(seq_values),
    	.repeats         = 0,
    	.end_delay       = 0
	};

	while (true) {
		nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
		k_msleep(10000);
		nrfx_pwm_stop(&m_pwm0, false);
		k_msleep(1000);
	}
}