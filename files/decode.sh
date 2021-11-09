#!/bin/bash

filename_with_underscore=$(sed 's/-/_/g' <<< $1)

opusenc $1.wav $1.opus --bitrate 32 --framesize 5 --downmix-mono --discard-comments --discard-pictures
bin2c -o ../src/audio/${filename_with_underscore}.h -name "$filename_with_underscore" $1.opus
