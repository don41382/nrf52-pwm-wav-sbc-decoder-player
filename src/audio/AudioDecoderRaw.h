#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "AudioDecoder.h"

class AudioDecoderRaw : public AudioDecoder {
    public:
        AudioDecoderRaw(const unsigned char * data, size_t length, AudioSampleRate sampleRate) : AudioDecoder(data, length) {
            this->sampleRate = sampleRate;
        };
        DecoderResult decode(unsigned char * buffer, size_t length, size_t * totalDecoded) override {
            this->sampleRate = sampleRate;
            if (length > this->rawLength - this->rawIdx) {
                *totalDecoded = max((int)this->rawLength - this->rawIdx, 0); 
            } else {
                *totalDecoded = length;
            }
            memcpy(buffer, this->rawData, *totalDecoded);
            this->rawIdx += *totalDecoded;
            return SUCCESS;
        };
};