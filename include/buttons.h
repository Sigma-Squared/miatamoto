#ifndef _BUTTONS_H
#define _BUTTONS_H
#include <Arduino.h>
#include <DEBUG.h>
#include <BluetoothA2DPSink.h>

#define BTN_PLAY_PAUSE_PIN 4
#define BTN_PREV_PIN 16
#define BTN_NEXT_PIN 17
#define BTN_ON_OFF_PIN 5
#define MEDIA_ENABLED_PIN 21

enum
{
    BTN_PLAY_PAUSE,
    BTN_PREV,
    BTN_NEXT,
    BTN_ON_OFF,
    BTN_COUNT
};

#define DEBOUNCE_TIME 10 // milliseconds

volatile bool btn_flags[BTN_COUNT]{};
unsigned long btn_last_time[BTN_COUNT]{};
bool media_paused = false;
bool media_enabled = true;

void IRAM_ATTR play_pause_callback() { btn_flags[BTN_PLAY_PAUSE] = true; }
void IRAM_ATTR prev_callback() { btn_flags[BTN_PREV] = true; }
void IRAM_ATTR next_callback() { btn_flags[BTN_NEXT] = true; }
void IRAM_ATTR on_off_callback() { btn_flags[BTN_ON_OFF] = true; }

void setup_buttons()
{
    pinMode(BTN_PLAY_PAUSE_PIN, INPUT_PULLUP);
    pinMode(BTN_PREV_PIN, INPUT_PULLUP);
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_ON_OFF_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BTN_PLAY_PAUSE_PIN), play_pause_callback, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_PREV_PIN), prev_callback, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_NEXT_PIN), next_callback, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_ON_OFF_PIN), on_off_callback, FALLING);
    pinMode(MEDIA_ENABLED_PIN, OUTPUT);
    digitalWrite(MEDIA_ENABLED_PIN, media_enabled);
}

void button_loop(BluetoothA2DPSink *const a2dp_sink)
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
                    DEBUG_PRINTLN(F("Play/Pause button pressed"));
                    if (media_paused)
                        a2dp_sink->play();
                    else
                        a2dp_sink->pause();
                    media_paused = !media_paused;
                    break;
                case BTN_PREV:
                    DEBUG_PRINTLN(F("Previous button pressed"));
                    a2dp_sink->previous();
                    break;
                case BTN_NEXT:
                    DEBUG_PRINTLN(F("Next button pressed"));
                    a2dp_sink->next();
                    break;
                case BTN_ON_OFF:
                    DEBUG_PRINTLN(F("On/Off button pressed"));
                    media_enabled = !media_enabled;
                    digitalWrite(MEDIA_ENABLED_PIN, media_enabled);
                    break;
                }
            }
        }
    }
}
#endif
