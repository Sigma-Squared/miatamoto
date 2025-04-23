#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#include <marquee.h>

// ALL USED PINS
// I2S A2DP
constexpr uint8_t I2S_BCK = 14;
constexpr uint8_t I2S_WS_LRCK = 15;
constexpr uint8_t I2S_DATA_DIN = 22;
constexpr uint8_t I2S_MCK = 0; // optional

// I2S DISPLAY
constexpr uint8_t DISPLAY_SDA = 19;
constexpr uint8_t DISPLAY_SCL = 18;

// BUTTON CONTROLS
constexpr uint8_t MEDIA_ENABLED_PIN_BAR = 2; // GPIO pin for media enabled MOSFET
constexpr uint8_t LIGHTS_ENABLED_PIN = 13;   // GPIO pin light MOSFET
constexpr uint8_t BUTTON_PINS[] = {
    15, // BTN_PLAY_PAUSE
    22, // BTN_PREV
    4,  // BTN_NEXT
    5,  // BTN_ON_OFF
    21  // BTN_MISC
};

TwoWire oledI2C = TwoWire(1);
Adafruit_SSD1306 display(128, 32, &oledI2C, -1);

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

constexpr uint8_t CHARLIM = 11;
constexpr uint8_t EDGE_STALL = 5;

bool splash = true;
bool media_enabled = true;
bool media_paused = true;
uint8_t esp_mac[6] = {0};

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