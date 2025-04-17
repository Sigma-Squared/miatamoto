#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#include <marquee.h>

TwoWire oledI2C = TwoWire(1);
Adafruit_SSD1306 display(128, 32, &oledI2C, -1);

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

constexpr uint8_t CHARLIM = 11;
constexpr uint8_t EDGE_STALL = 5;

bool splash = true;
bool media_enabled = true;

struct AVRCMetadata
{
    MarqueeText<CHARLIM, EDGE_STALL> title_marquee;
    MarqueeText<CHARLIM * 2, EDGE_STALL> artist_marquee;
    MarqueeText<CHARLIM * 2, EDGE_STALL> album_marquee;
    uint32_t playtime_ms;
    uint32_t play_start;
    AVRCMetadata(const uint8_t *const clock) : playtime_ms(0), play_start(0), title_marquee(clock), artist_marquee(clock), album_marquee(clock) {}
};

uint8_t global_marquee_clock = 0;
static struct AVRCMetadata meta(&global_marquee_clock);

void render();
void display_large(const char *text);

#endif