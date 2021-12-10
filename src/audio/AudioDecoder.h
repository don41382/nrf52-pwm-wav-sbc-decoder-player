#pragma once

#include <stdint.h>
#include <stdio.h>
#include <functional>

typedef enum {
    SAMPLERATE_31K = 1,           ///< use 31250 Hz sampling rate
    SAMPLERATE_16K = 2,           ///< use 15625 Hz sampling rate
    SAMPLERATE_8K  = 4            ///< use 7812.5 Hz sampling rate
} AudioSampleRate;

typedef enum {
    SUCCESS = 0,
    MISSING_FRAME = 1,
    FREQUENCY_TO_HIGH = 2
} DecoderResult;

class AudioDecoder {

    protected:
        const unsigned char * rawData = NULL;
        size_t rawLength;
        uint16_t rawIdx;

        AudioSampleRate sampleRate;
    public:
        AudioDecoder(const unsigned char * data, size_t length) {
            this->rawData = data;
            this->rawLength = length;
            this->rawIdx = 0;
        }
        virtual DecoderResult decode(unsigned char * buffer, size_t bufferLength, size_t * totalDecoded);
        void reset() { this->rawIdx = 0; }
        AudioSampleRate getSampleRate() { return this->sampleRate; };
};