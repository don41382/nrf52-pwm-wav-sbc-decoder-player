#pragma once

#include <stdint.h>
#include <stdio.h>
#include "AudioDecoder.h"
#include "sbc.h"

// mimimum for mono must be 256
#define SBC_BUFFER_SIZE 256

class AudioDecoderSbc : public AudioDecoder {
    private:
        sbc_t sbc;
        bool init = false;
        unsigned char sbcBuffer[SBC_BUFFER_SIZE] = {0};
        unsigned short getShort(unsigned char* array, int offset) { 
            return (short)(((short)array[offset]) << 8) | array[offset + 1];
        }
        int copy16PCMto8bit(uint8_t * result, unsigned char * buffer, int bufferSize, int resultPos) {
            for (int i=0; i<bufferSize/2; i++) {
                unsigned short pcm16 = getShort(buffer, i*2);
                result[resultPos + i] = (uint8_t) ((pcm16)>>8);
            }
            return bufferSize/2;
        };
        DecoderResult setSampleRate() {
            int framelen = sbc_decode(&sbc, this->rawData, this->rawLength, sbcBuffer, SBC_BUFFER_SIZE, nullptr);
            if (framelen > 0) {
                switch (this->sbc.frequency) {
                    case SBC_FREQ_16000:
                        this->sampleRate = AudioSampleRate::SAMPLERATE_16K;
                        return SUCCESS;
                    case SBC_FREQ_32000:
                        this->sampleRate = AudioSampleRate::SAMPLERATE_31K;
                        return SUCCESS;
                    default:
                        return FREQUENCY_TO_HIGH;
                }
            } else {
                return MISSING_FRAME;
            }
        };
    public:
        AudioDecoderSbc(const unsigned char * data, size_t length) : AudioDecoder(data, length) {
            sbc_init(&sbc, 0L);
            sbc.endian = SBC_BE;
        };
        DecoderResult decode(unsigned char * pcmOut, size_t pcmOutSize, size_t * totalDecoded) override {
            if (this->rawIdx < (uint16_t)this->rawLength) {
                if (!this->init) {
                    DecoderResult res = setSampleRate();
                    if (res < 0) {
                        return res;
                    } else {
                        this->init = true;
                    }
                }
                
                *totalDecoded = 0;
                ssize_t framelen = 0;
                do {
                    size_t written;
                    framelen = sbc_decode(&sbc, this->rawData + this->rawIdx, this->rawLength - this->rawIdx, sbcBuffer, SBC_BUFFER_SIZE, &written);
                    if (framelen > 0) {
                        this->rawIdx += framelen;
                        *totalDecoded += copy16PCMto8bit(pcmOut, sbcBuffer, written, *totalDecoded);
                    }
                } while (*totalDecoded < pcmOutSize && framelen > 0);
                
                return SUCCESS;
            } else {
                return SUCCESS;
            }
        };
};