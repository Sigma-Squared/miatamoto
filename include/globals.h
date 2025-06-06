#ifndef GLOBALS_H
#define GLOBALS_H

#include <marquee.h>
#include <observable.h>

// ALL USED PINS
// I2S A2DP
constexpr uint8_t I2S_BCK = 12;
constexpr uint8_t I2S_WS_LRCK = 14;
constexpr uint8_t I2S_DATA_DIN = 27;
constexpr uint8_t I2S_MCK = 0; // optional

// I2S DISPLAY
constexpr uint8_t DISPLAY_SDA = 19;
constexpr uint8_t DISPLAY_SCL = 18;

// BUTTON CONTROLS
constexpr uint8_t MEDIA_ENABLED_PIN_BAR = 13; // GPIO pin for media enabled MOSFET
constexpr uint8_t LIGHTS_ENABLED_PIN = 2;     // GPIO pin light MOSFET
constexpr uint8_t BUTTON_PINS[] = {
    15, // BTN_PLAY_PAUSE
    22, // BTN_PREV
    4,  // BTN_NEXT
    5,  // BTN_ON_OFF
    21  // BTN_MISC
};

// GLOBAL VARIABLES
extern Observable<bool> media_enabled;
extern bool media_paused;
extern uint8_t esp_mac[6];

constexpr uint8_t CHARLIM = 11;
constexpr uint8_t EDGE_STALL = 5;
struct AVRCMetadata
{
    MarqueeText<CHARLIM, EDGE_STALL> title_marquee;
    MarqueeText<CHARLIM * 2, EDGE_STALL> artist_marquee;
    MarqueeText<CHARLIM * 2, EDGE_STALL> album_marquee;
    uint32_t playtime_ms;
    uint32_t play_start;
    AVRCMetadata(const uint8_t *const clock) : playtime_ms(0), play_start(0), title_marquee(clock), artist_marquee(clock), album_marquee(clock) {}
};
extern struct AVRCMetadata meta;
extern uint8_t global_marquee_clock;

#endif