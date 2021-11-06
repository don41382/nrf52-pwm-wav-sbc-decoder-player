
#ifndef OPUS_AUDIO_CONFIG_H_
#define OPUS_AUDIO_CONFIG_H_

#define CONFIG_OPUS_MODE                        CONFIG_OPUS_MODE_SILK

#define CONFIG_OPUS_MODE_CELT                   (1 << 0)
#define CONFIG_OPUS_MODE_SILK                   (1 << 1)
#define CONFIG_OPUS_MODE_HYBRID                 (CONFIG_OPUS_MODE_CELT | CONFIG_OPUS_MODE_SILK)

#endif