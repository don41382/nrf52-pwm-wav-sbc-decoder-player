#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "AudioDecoder.h"

class AudioDecoderRaw : public AudioDecoder {
    using AudioDecoder::AudioDecoder;
    public:
        int decode(unsigned char * buffer, size_t length) override {
            uint16_t maxLength;
            if (length > this->rawLength - this->rawIdx) {
                maxLength = max((int)this->rawLength - this->rawIdx, 0);
            } else {
                maxLength = length;
            }
            memcpy(buffer, this->rawData, maxLength);
            this->rawIdx += maxLength;
            return maxLength;
        };
};