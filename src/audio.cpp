#include <Wire.h>
#include <driver/i2s_std.h>

#include <Talkie.h>
#include <TalkieUtils.h>
#include <Vocab_US_Large.h>

#include "audio.h"
#include "config.h"

// The I2S bus runs at twice Talkie's 8 kHz sample rate, samples are interpolated.
#define I2S_SAMPLE_RATE 16000
#define ES8311_ADDR_LOW 0x18
#define ES8311_ADDR_HIGH 0x19
// Software volume, 0-256.
#define AUDIO_VOLUME 200
// DAC volume register, 0xBF is 0 dB.
#define ES8311_DAC_VOLUME 0xBF

#define RING_SIZE 1024
#define CHUNK 64
#define PREBUFFER 128
#define PA_OFF_DELAY_MS 600

static Talkie voice(true, false);
static i2s_chan_handle_t tx_handle = NULL;
static uint8_t codec_addr = ES8311_ADDR_LOW;
static bool audio_ready = false;
static uint32_t last_speech_ms = 0;

// Single producer (Talkie ISR), single consumer (I2S task) ring buffer.
static volatile uint8_t ring[RING_SIZE];
static volatile uint16_t ring_head = 0;
static volatile uint16_t ring_tail = 0;

/**
 * Called by Talkie from the 8 kHz timer interrupt, see lib/Talkie.
 */
void IRAM_ATTR talkieOutputSample(uint8_t sample) {
	uint16_t next = (ring_head + 1) % RING_SIZE;
	if (next != ring_tail) {
		ring[ring_head] = sample;
		ring_head = next;
	}
}

static uint16_t ring_level() {
	return (ring_head + RING_SIZE - ring_tail) % RING_SIZE;
}

static bool codec_write(uint8_t reg, uint8_t value) {
	Wire.beginTransmission(codec_addr);
	Wire.write(reg);
	Wire.write(value);
	return Wire.endTransmission() == 0;
}

static int codec_read(uint8_t reg) {
	Wire.beginTransmission(codec_addr);
	Wire.write(reg);
	if (Wire.endTransmission(false) != 0) {
		return -1;
	}
	if (Wire.requestFrom((int) codec_addr, 1) != 1) {
		return -1;
	}
	return Wire.read();
}

/**
 * ES8311 as I2S slave, MCLK = 256 * 16 kHz from the MCLK pin, 16 bit, DAC only.
 */
static bool codec_init() {
	Wire.begin(CODEC_SDA, CODEC_SCL, 400000);

	// Chip ID register 0xFD reads 0x83.
	codec_addr = ES8311_ADDR_LOW;
	if (codec_read(0xFD) != 0x83) {
		codec_addr = ES8311_ADDR_HIGH;
		if (codec_read(0xFD) != 0x83) {
			ets_printf("ES8311 not found.\n");
			return false;
		}
	}

	codec_write(0x44, 0x08);
	codec_write(0x44, 0x08);
	codec_write(0x01, 0x30);
	codec_write(0x02, 0x00);
	codec_write(0x03, 0x10);
	codec_write(0x16, 0x24);
	codec_write(0x04, 0x10);
	codec_write(0x05, 0x00);
	codec_write(0x0B, 0x00);
	codec_write(0x0C, 0x00);
	codec_write(0x10, 0x1F);
	codec_write(0x11, 0x7F);
	// Power on, slave mode.
	codec_write(0x00, 0x80);
	// Enable all clocks, MCLK from pin.
	codec_write(0x01, 0x3F);

	// Clock coefficients for MCLK 4.096 MHz / 16 kHz.
	codec_write(0x02, 0x00); // pre_div 1, pre_multi 1
	codec_write(0x03, 0x10); // fs_mode 0, adc_osr 0x10
	codec_write(0x04, 0x20); // dac_osr 0x20
	codec_write(0x05, 0x00); // adc_div 1, dac_div 1
	codec_write(0x06, 0x03); // bclk_div 4
	codec_write(0x07, 0x00); // lrck_h
	codec_write(0x08, 0xFF); // lrck_l

	// I2S, 16 bit.
	codec_write(0x09, 0x0C);
	codec_write(0x0A, 0x0C);

	// Power up analog and DAC.
	codec_write(0x0D, 0x01);
	codec_write(0x0E, 0x02);
	codec_write(0x12, 0x00);
	codec_write(0x13, 0x10);
	codec_write(0x1C, 0x6A);
	codec_write(0x37, 0x08);
	codec_write(0x32, ES8311_DAC_VOLUME);
	return true;
}

static bool i2s_init() {
	i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
	chan_cfg.auto_clear = true;
	if (i2s_new_channel(&chan_cfg, &tx_handle, NULL) != ESP_OK) {
		return false;
	}

	i2s_std_config_t cfg = {};
	cfg.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE);
	cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
	cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
	cfg.gpio_cfg.mclk = (gpio_num_t) I2S_MCLK;
	cfg.gpio_cfg.bclk = (gpio_num_t) I2S_BCLK;
	cfg.gpio_cfg.ws = (gpio_num_t) I2S_LRCK;
	cfg.gpio_cfg.dout = (gpio_num_t) I2S_DOUT;
	cfg.gpio_cfg.din = I2S_GPIO_UNUSED;

	if (i2s_channel_init_std_mode(tx_handle, &cfg) != ESP_OK) {
		return false;
	}
	return i2s_channel_enable(tx_handle) == ESP_OK;
}

/**
 * Feeds the I2S bus: silence when idle, interpolated Talkie samples when speaking.
 */
static void audio_task(void *) {
	static int16_t frames[CHUNK * 4]; // CHUNK samples -> 2 stereo frames each
	int16_t previous = 0;
	bool playing = false;
	uint16_t last_level = 0;

	for (;;) {
		uint16_t level = ring_level();

		if (!playing && level > 0) {
			// Wait for a prebuffer, or for the tail of an utterance to stop growing.
			playing = level >= PREBUFFER || level == last_level;
		}
		last_level = level;

		size_t count = 0;
		if (playing) {
			count = level < CHUNK ? level : CHUNK;
			if (count == 0) {
				playing = false;
			}
		}

		if (count == 0) {
			memset(frames, 0, sizeof(frames));
			size_t written;
			// 8 ms of silence (64 samples at 8 kHz).
			i2s_channel_write(tx_handle, frames, CHUNK * 4 * sizeof(int16_t), &written, portMAX_DELAY);
			previous = 0;
			continue;
		}

		for (size_t i = 0; i < count; i++) {
			int16_t current = (int16_t) (((int) ring[ring_tail] - 128) << 8);
			current = (int16_t) (((int) current * AUDIO_VOLUME) >> 8);
			ring_tail = (ring_tail + 1) % RING_SIZE;

			int16_t middle = (int16_t) (((int) previous + current) / 2);
			frames[i * 4 + 0] = middle;
			frames[i * 4 + 1] = middle;
			frames[i * 4 + 2] = current;
			frames[i * 4 + 3] = current;
			previous = current;
		}
		size_t written;
		i2s_channel_write(tx_handle, frames, count * 4 * sizeof(int16_t), &written, portMAX_DELAY);
	}
}

void setup_audio() {
	pinMode(AUDIO_PA_CTRL, OUTPUT);
	digitalWrite(AUDIO_PA_CTRL, LOW);

	if (!i2s_init()) {
		ets_printf("I2S init failed.\n");
		return;
	}
	// MCLK is running now, the codec needs it for its register setup.
	if (!codec_init()) {
		return;
	}

	xTaskCreate(audio_task, "audio", 4096, NULL, 3, NULL);
	audio_ready = true;
	ets_printf("Audio ready.\n");
}

void audio_say_remaining(int seconds) {
	if (!audio_ready || seconds <= 0) {
		return;
	}

	digitalWrite(AUDIO_PA_CTRL, HIGH);
	last_speech_ms = millis();

	// Drop whatever is still queued so the announcement is never late.
	voice.stop();

	// Numbers only: 270 -> "four thirty", 300 -> "five", 30 -> "thirty".
	int minutes = seconds / 60;
	int secs = seconds % 60;
	if (minutes > 0) {
		sayQNumber(&voice, minutes);
	}
	if (secs > 0) {
		sayQNumber(&voice, secs);
	}
}

void audio_loop() {
	if (!audio_ready) {
		return;
	}
	if (voice.isTalking() || ring_level() > 0) {
		last_speech_ms = millis();
	} else if (digitalRead(AUDIO_PA_CTRL) && millis() - last_speech_ms > PA_OFF_DELAY_MS) {
		digitalWrite(AUDIO_PA_CTRL, LOW);
	}
}
