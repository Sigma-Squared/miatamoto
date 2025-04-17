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

enum Buttons : uint8_t
{
    BTN_PLAY_PAUSE,
    BTN_PREV,
    BTN_NEXT,
    BTN_ON_OFF,
    BTN_COUNT
};

constexpr unsigned long DEBOUNCE_TIME = 150; // milliseconds

volatile bool btn_flags[BTN_COUNT]{};
unsigned long btn_last_time[BTN_COUNT]{};
bool media_paused = false;

void IRAM_ATTR on_play_pause() { btn_flags[BTN_PLAY_PAUSE] = true; }
void IRAM_ATTR on_prev() { btn_flags[BTN_PREV] = true; }
void IRAM_ATTR on_next() { btn_flags[BTN_NEXT] = true; }
void IRAM_ATTR on_on_off() { btn_flags[BTN_ON_OFF] = true; }

void setup_buttons()
{
    pinMode(BTN_PLAY_PAUSE_PIN, INPUT_PULLUP);
    pinMode(BTN_PREV_PIN, INPUT_PULLUP);
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_ON_OFF_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BTN_PLAY_PAUSE_PIN), on_play_pause, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_PREV_PIN), on_prev, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_NEXT_PIN), on_next, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_ON_OFF_PIN), on_on_off, FALLING);
    pinMode(MEDIA_ENABLED_PIN, OUTPUT);
    digitalWrite(MEDIA_ENABLED_PIN, media_enabled);
}

void button_loop()
{
    for (uint8_t i = 0; i < BTN_COUNT; i++)
    {
        if (btn_flags[i])
        {
            btn_flags[i] = false;
            auto now = millis();
            if (now - btn_last_time[i] >= DEBOUNCE_TIME)
            {
                btn_last_time[i] = now;
                switch (i)
                {
                case BTN_PLAY_PAUSE:
                    DEBUG_PRINTLN("Play/Pause button pressed");
                    // if (media_paused)
                    //     a2dp_sink->play();
                    // else
                    //     a2dp_sink->pause();
                    // media_paused = !media_paused;
                    break;
                case BTN_PREV:
                    DEBUG_PRINTLN("Previous button pressed");
                    // a2dp_sink->previous();
                    break;
                case BTN_NEXT:
                    DEBUG_PRINTLN("Next button pressed");
                    // a2dp_sink->next();
                    break;
                case BTN_ON_OFF:
                    DEBUG_PRINTLN("On/Off button pressed");
                    media_enabled = !media_enabled;
                    digitalWrite(MEDIA_ENABLED_PIN, media_enabled);
                    if (!media_enabled)
                    {
                        display_large("AMP OFF");
                    }

                    break;
                }
            }
        }
    }
}
#endif
