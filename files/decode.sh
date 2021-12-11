#!/bin/bash

PARAM1=$1
PARAM2=$2

if [[ -z $PARAM1 || -z $PARAM2 ]]; then
    echo "./decoder [-raw|-sbc] <filename>"
    exit
fi

POSITIONAL=()
while [[ $# -gt 0 ]]; do
  case $PARAM1 in
    -s|-sbc)
	  AUDIO="sbc"
      FILE_NAME="$PARAM2"
      shift # past argument
      shift # past value
      ;;
	-w|-raw)
	  AUDIO="raw"
      FILE_NAME="$PARAM2"
      shift # past argument
      shift # past value
      ;;
  esac
done

filename_with_underscore=$(sed 's/-/_/g' <<< $FILE_NAME)
name=${filename_with_underscore%.*}

case $AUDIO in
	"sbc")
		echo "+ processing sbc file ..."
		ffmpeg -i $FILE_NAME -acodec sbc -f s16be -ar 16000 -b:a 32k -ac 1 tmp.raw
		bin2c -o ../src/samples/sample_${name}_sbc.h -name "sample_${name}_sbc" tmp.raw
		;;
	"raw")
		echo "+ processing raw file ..."
		ffmpeg -i $FILE_NAME -acodec pcm_s8 -f s8 -ar 16000 -ac 1 tmp.raw
		bin2c -o ../src/samples/sample_${name}_raw.h -name "sample_${name}_raw" tmp.raw
		;;
esac
		
rm tmp.raw
