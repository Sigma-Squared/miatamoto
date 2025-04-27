#include <Arduino.h>

#include <DEBUG.h>
#include <buttons.h>
#include <a2dpsink.h>
#include <globals.h>
#include <display.h>

constexpr uint8_t LIGHT_BRIGHTNESS = 127;    // 0-255, 0 = off, 255 = full brightness
constexpr unsigned long DEBOUNCE_TIME = 350; // milliseconds

bool lights_enabled = false;

enum Buttons : uint8_t
{
    BTN_PLAY_PAUSE,
    BTN_PREV,
    BTN_NEXT,
    BTN_ON_OFF,
    BTN_MISC,
    BTN_COUNT
};
// in buttons.h (or above loop in .ino)

volatile bool btn_flags[BTN_COUNT] = {false};
unsigned long btn_last_time[BTN_COUNT] = {0};

void IRAM_ATTR isr_play_pause() { btn_flags[BTN_PLAY_PAUSE] = true; }
void IRAM_ATTR isr_prev() { btn_flags[BTN_PREV] = true; }
void IRAM_ATTR isr_next() { btn_flags[BTN_NEXT] = true; }
void IRAM_ATTR isr_on_off() { btn_flags[BTN_ON_OFF] = true; }
void IRAM_ATTR isr_misc() { btn_flags[BTN_MISC] = true; }

void setup_buttons()
{
    for (int btn = 0; btn < BTN_COUNT; btn++)
        pinMode(BUTTON_PINS[btn], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_PLAY_PAUSE]), isr_play_pause, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_PREV]), isr_prev, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_NEXT]), isr_next, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_ON_OFF]), isr_on_off, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PINS[BTN_MISC]), isr_misc, FALLING);
    pinMode(MEDIA_ENABLED_PIN_BAR, OUTPUT);
    pinMode(LIGHTS_ENABLED_PIN, OUTPUT);
    digitalWrite(MEDIA_ENABLED_PIN_BAR, !media_enabled.get());
    ledcSetup(0, 1000, 8);
    ledcAttachPin(LIGHTS_ENABLED_PIN, 0);
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
            media_enabled.set(!media_enabled.get());
            digitalWrite(MEDIA_ENABLED_PIN_BAR, !media_enabled.get());
            DEBUG_PRINTLN("On/Off button pressed");
            break;
        case BTN_MISC:
            lights_enabled = !lights_enabled;
            ledcWrite(0, lights_enabled ? LIGHT_BRIGHTNESS : 0);
            DEBUG_PRINTLN("Light button pressed");
            break;
        }
    }
}
