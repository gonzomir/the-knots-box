#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

/**
 * Initialise the ES8311 codec, I2S output and Talkie speech engine.
 */
void setup_audio();

/**
 * Announce the remaining time, e.g. "four minutes thirty seconds". Non-blocking.
 */
void audio_say_remaining(int seconds);

/**
 * Housekeeping, call from loop(). Switches the amplifier off after speech.
 */
void audio_loop();

#endif
