#!/bin/bash

filename_with_underscore=$(sed 's/-/_/g' <<< $1)
name=${filename_with_underscore%.*}


ffmpeg -i $1 -acodec sbc -f s16be -ar 16000 -b:a 32k -ac 1 tmp.raw
#ffmpeg -i $1 -acodec sbc -f u8 -ar 16000 -b:a 32k tmp.raw
bin2c -o ../src/samples/${name}.h -name "$name" tmp.raw
rm tmp.raw
