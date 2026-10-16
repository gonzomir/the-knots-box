#include <Arduino.h>

#define GNSS_RX 1
#define GNSS_TX 2
#define GNSS_PPS 3
#define GNSS_EN 17
#define GNSS_EN_GPIO GPIO_NUM_17

#define MAIN_BTN 0
#define MAIN_BTN_GPIO GPIO_NUM_0

#define MODE_BTN 18
#define NODE_BTN_GPIO GPIO_NUM_18

#define BATTERY_STATUS 4
#define BATTERY_STATUS_ADC_CHANNEL ADC1_CHANNEL_3
#define BATTERY_STATUS_ATTENUATION ADC_ATTEN_DB_12

#define DISPLAY_MOSI 12
#define DISPLAY_SCL 11
#define DISPLAY_CS 40
#define DISPLAY_DC 5
#define DISPLAY_RST 41
#define DISPLAY_W 400
#define DISPLAY_H 300

// ES8311 audio codec (speaker): I2C control + I2S data.
#define CODEC_SDA 13
#define CODEC_SCL 14
#define I2S_MCLK 16
#define I2S_BCLK 9
#define I2S_LRCK 45
#define I2S_DOUT 8
#define AUDIO_PA_CTRL 46

// The I2S bus runs at twice Talkie's 8 kHz sample rate, samples are interpolated.
#define I2S_SAMPLE_RATE 16000
#define ES8311_ADDR_LOW 0x18
#define ES8311_ADDR_HIGH 0x19
// Software volume, 0-256.
#define AUDIO_VOLUME 200
// DAC volume register, 0xBF is 0 dB.
#define ES8311_DAC_VOLUME 0xBF

// Ring buffer between the Talkie interrupt and the I2S task, I2S chunk and prebuffer sizes in samples.
#define RING_SIZE 1024
#define CHUNK 64
#define PREBUFFER 128
// Delay before the amplifier is switched off after speech.
#define PA_OFF_DELAY_MS 600

#ifndef CONFIG_H
#define CONFIG_H

enum tkb_mode { speed, start };

extern bool should_sleep;
extern bool gps_is_ready;
extern bool do_read_gnss;
extern bool do_start_timer;
extern tkb_mode display_mode;
extern bool start_timer_started;

#endif
