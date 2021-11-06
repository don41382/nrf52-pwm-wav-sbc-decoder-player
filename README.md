# NRF52 Audio Player with PWM ouput & opus codec

MORE INFO HERE!

## Raw 8-bit PCM (no compression)

Requirements

- [ffmpeg](https://ffmpeg.org/)
- [bin2c](https://sourceforge.net/projects/bin2c/)

In order to play your file via the PWM, it mus be converted into a c file.

1. Convert your soundfile into a signed raw 8-bit PCM
   
   `# ffmpeg -i soundfile.mp3 -ar 15625 -f s8 soundfile.raw`

2. Convert the raw soundfile into a C file

   `# ./bin2c -o soundfile.h -name "soundfile" soundfile.raw` 

3. Use it inside your C project

## Opus (no compression)

Requirements

- [ffmpeg](https://ffmpeg.org/)
- [opus-tools](https://opus-codec.org/downloads/) (optional)
- [bin2c](https://sourceforge.net/projects/bin2c/)

1. Convert your soundfile into a wave file

   `# ffmpeg -i soundfile.mp3 soundfile.wav`

2. Encode your soundfile to a opus file. The `bitrate` and `framesize` have to match your decoder settings (`AUDIO_BITRATE_LIMIT` and `AUDIO_FRAME_SIZE_MS`)

   `# opusenc audiofile.wav audiofile.opus --bitrate 32 --framesize 5 --downmix-mono --discard-comments --discard-pictures`

3. Convert the soundfile into a C file

   `# 
