# NRF52 Audio Player with PWM output for raw wav & sbc codec files

This project allows you to playback *raw wave* and *sbc encoded* audio files via PWM output on NRF52 chips. It can also easily extended to add other decoders of your choice.

- Build on the latest [NRF connect SDK 1.7.0](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.7.0/)
- Supported is a sampling rate up to 32kHz and 8-bit resolution
- C++ is used

For testing, you can connect your headphone to the PWM output. In order to connect a speaker, you'll need an amplifier. A simple Class D Amplifier did the job for [me](https://siliconjunction.wordpress.com/2017/02/28/class-d-amplifier-for-the-arduino/).

A big thanks goes to Tamas Harczos. His initial [repository](https://sourceforge.net/u/newtom/profile/) helped me tones to make this working.

## Deployment

### Pre-Requirements

- latest NRF connect SDK 1.7.0
- NRF52 module with [PWM support](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fstruct_nrf52%2Fstruct%2Fnrf52.html) (I worked with the [NRF52840 Dongle](https://www.nordicsemi.com/Products/Development-hardware/nrf52840-dongle))

### Run

1. Checkout the repository

   `git clone git@github.com:don41382/nrf52-pwm-wav-sbc-decoder-player.git`

2. Build & flash with west

   `west flash`

## Encoding your Audio

I created a simple bash script (`files/decode.sh`) which helps you to convert your audio files into C header files.

### Required Installed Libraries

- [ffmpeg](https://ffmpeg.org/)
- [bin2c](https://sourceforge.net/projects/bin2c/)

### Encode your files

```
# cd files
# ./decode itsworking.wav
```

