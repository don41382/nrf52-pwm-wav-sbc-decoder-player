#pragma once

#include <stdint.h>
#include <stdio.h>
#include <functional>

class AudioDecoder {
    protected:
        const unsigned char * rawData = NULL;
        size_t rawLength;
        uint16_t rawIdx;
    public:
        AudioDecoder(const unsigned char * data, size_t length) {
            this->rawData = data;
            this->rawLength = length;
            this->rawIdx = 0;
        }
        virtual int decode(unsigned char * buffer, size_t bufferLength);
        void reset() { this->rawIdx = 0; }
};