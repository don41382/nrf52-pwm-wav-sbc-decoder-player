#pragma once

#include <zephyr.h>
#include <stdio.h>
#include "sbc.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_SIZE 512

unsigned short getShort(unsigned char* array, int offset)
{
    return (short)(((short)array[offset]) << 8) | array[offset + 1];
}

int copy16PCMto8Result(uint8_t * result, unsigned char * buffer, int bufferSize, int resultPos) {
    for (int i=0; i<bufferSize/2; i++) {
        unsigned short pcm16 = getShort(buffer, i*2);
        result[resultPos + i] = (uint8_t) ((pcm16)>>8);
    }
    return bufferSize/2;
}

void decodeSbcFile(const unsigned char * sbcData, size_t sbcDataLength, uint8_t * pcmResult) {   
    sbc_t sbc;
    sbc_init(&sbc, 0L);
    sbc.endian = SBC_BE;

    unsigned char * sbcBuffer = (unsigned char *)k_malloc(BUF_SIZE);
    size_t outputPCMLength;
    int framelen = 0;
    int sbcPos = 0;
    int pcmPos = 0;

    do {
        sbcPos += framelen;
        framelen = sbc_decode(&sbc, sbcData + sbcPos, sbcDataLength - sbcPos, sbcBuffer, BUF_SIZE, &outputPCMLength);
        if (framelen > 0) { 
            pcmPos += copy16PCMto8Result(pcmResult, sbcBuffer, outputPCMLength, pcmPos);
        }
    } while (framelen > 0);
    printk("size: %d\n",pcmPos);
    k_free(sbcBuffer);
}

#ifdef __cplusplus
}
#endif
