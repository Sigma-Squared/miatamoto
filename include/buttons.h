#ifndef BUTTONS_H
#define BUTTONS_H
#include <Arduino.h>
#include <BluetoothA2DPSink.h>

#include <DEBUG.h>
#include <globals.h>

constexpr uint8_t BTN_PLAY_PAUSE_PIN = 32;
constexpr uint8_t BTN_PREV_PIN = 33;
constexpr uint8_t BTN_NEXT_PIN = 25;
constexpr uint8_t BTN_ON_OFF_PIN = 26;
constexpr uint8_t MEDIA_ENABLED_PIN = 21;

constexpr uint8_t DEBOUNCE_TIME = 250; // milliseconds

enum Buttons : uint8_t
{
    BTN_PLAY_PAUSE,
    BTN_PREV,
    BTN_NEXT,
    BTN_ON_OFF,
    BTN_COUNT
};
// in buttons.h (or above loop in .ino)

constexpr uint8_t BUTTON_PINS[] = {
    BTN_PLAY_PAUSE_PIN, // 32
    BTN_PREV_PIN,       // 33
    BTN_NEXT_PIN,       // 25
    BTN_ON_OFF_PIN      // 26
};

volatile bool btn_flags[BTN_COUNT] = {false};
unsigned long btn_last_time[BTN_COUNT] = {0};

bool media_paused = true;

// One ISR *per* button index:
void IRAM_ATTR isrPlayPause() { btn_flags[BTN_PLAY_PAUSE] = true; }
void IRAM_ATTR isrPrev() { btn_flags[BTN_PREV] = true; }
void IRAM_ATTR isrNext() { btn_flags[BTN_NEXT] = true; }
void IRAM_ATTR isrOnOff() { btn_flags[BTN_ON_OFF] = true; }

void setup_buttons()
{
    for (int i = 0; i < BTN_COUNT; i++)
    {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }
    attachInterrupt(digitalPinToInterrupt(BTN_PLAY_PAUSE_PIN),
                    isrPlayPause, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_PREV_PIN),
                    isrPrev, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_NEXT_PIN),
                    isrNext, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_ON_OFF_PIN),
                    isrOnOff, FALLING);
}

// then in loop(), exactly the same debounce/run logic you already have:
void button_loop()
{
    unsigned long now = millis();
    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        if (!btn_flags[i])
            continue;
        btn_flags[i] = false;

        if (now - btn_last_time[i] < DEBOUNCE_TIME)
            continue;
        btn_last_time[i] = now;

        if (digitalRead(BUTTON_PINS[i]) == HIGH)
            continue;

        switch (i)
        {
        case BTN_PLAY_PAUSE:
            if (media_paused)
                a2dp_sink.play();
            else
                a2dp_sink.pause();
            media_paused = !media_paused;
            DEBUG_PRINTLN("Play/Pause button pressed");
            break;
        case BTN_PREV:
            a2dp_sink.previous();
            DEBUG_PRINTLN("Previous button pressed");
            break;
        case BTN_NEXT:
            a2dp_sink.next();
            DEBUG_PRINTLN("Next button pressed");
            break;
        case BTN_ON_OFF:
            media_enabled = !media_enabled;
            digitalWrite(MEDIA_ENABLED_PIN, media_enabled);
            if (!media_enabled)
                display_large("AMP OFF");
            DEBUG_PRINTLN("On/Off button pressed");
            break;
        }
    }
}

#endif
