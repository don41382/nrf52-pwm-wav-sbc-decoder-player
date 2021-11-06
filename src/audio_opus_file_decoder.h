#pragma once

#include <zephyr.h>
#include <stdint.h>
#include "opusfile.h"
#include "nrftalk_opus.h"
#include "nrf.h"
#include "app_error.h"

int opus_mem_pos = 0;

void play() {
    int err = 0;
    OggOpusFile * oggFile = op_open_memory(nrfistalking_opus_8kHz_32k, nrfistalking_opus_8kHz_32k_size, &err);
    if (err == 0) {
        printk("open opus file worked!\n");
    } else {
        printk("error during opus file opening: %d\n", err);
    }
    opus_int16 data[1000] = {0};
    int sampleCount = op_read(oggFile, (opus_int16 *)&data, 1000, NULL);
    if (sampleCount < 0) {
        printk("error during reading ogg file: %d\n", sampleCount);
    } else {
        printk("%d samples read\n", sampleCount);
    }
}