#ifndef BUTTONS_H
#define BUTTONS_H
#include <Arduino.h>
#include <BluetoothA2DPSink.h>

#include <DEBUG.h>
#include <globals.h>

constexpr uint8_t MEDIA_ENABLED_PIN = 21; // GPIO pin for media enabled

constexpr unsigned long DEBOUNCE_TIME = 350; // milliseconds

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
    32, // BTN_PLAY_PAUSE
    33, // BTN_PREV
    25, // BTN_NEXT
    26  // BTN_ON_OFF
};

volatile bool btn_flags[BTN_COUNT] = {false};
unsigned long btn_last_time[BTN_COUNT] = {0};

void IRAM_ATTR isr_play_pause() { btn_flags[BTN_PLAY_PAUSE] = true; }
void IRAM_ATTR isr_prev() { btn_flags[BTN_PREV] = true; }
void IRAM_ATTR isr_next() { btn_flags[BTN_NEXT] = true; }
void IRAM_ATTR isr_on_off() { btn_flags[BTN_ON_OFF] = true; }

void setup_buttons()
{
    for (int btn = 0; btn < BTN_COUNT; btn++)
        pinMode(BUTTON_PINS[btn], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_PLAY_PAUSE]), isr_play_pause, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_PREV]), isr_prev, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_NEXT]), isr_next, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_ON_OFF]), isr_on_off, FALLING);
    pinMode(MEDIA_ENABLED_PIN, OUTPUT);
    digitalWrite(MEDIA_ENABLED_PIN, media_enabled);
}

// then in loop(), exactly the same debounce/run logic you already have:
void button_loop()
{
    unsigned long now = millis();
    for (uint8_t btn = 0; btn < BTN_COUNT; btn++)
    {
        if (!btn_flags[btn])
            continue;
        btn_flags[btn] = false;

        // time check (debounce)
        if (now - btn_last_time[btn] < DEBOUNCE_TIME)
            continue;
        btn_last_time[btn] = now;

        // crosstalk prevention
        if (digitalRead(BUTTON_PINS[btn]) != LOW)
            continue;

        switch (btn)
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
