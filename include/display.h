#ifndef DISPLAY_H
#define DISPLAY_H
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DEBUG.h>
#include <globals.h>

constexpr uint8_t DATALIM = 100;
constexpr uint8_t FONT_WIDTH = 3;
constexpr uint8_t FONT_HEIGHT = 4;

void render()
{
    if (splash || !media_enabled)
        return;
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println(meta.title_marquee.rendered_text);
    display.setTextSize(1);
    display.println(meta.album_marquee.rendered_text);
    display.println(meta.artist_marquee.rendered_text);
    if (meta.playtime_ms > 0)
    {
        uint32_t elapsed = millis() - meta.play_start;
        uint32_t progress = (elapsed * 128) / meta.playtime_ms;
        display.fillRect(0, 31, progress, 2, SSD1306_WHITE);
    }
    display.display();
}

void task_shift_text(void *_)
{
    for (;;)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        global_marquee_clock++;
        meta.title_marquee.marquee_step();
        meta.artist_marquee.marquee_step();
        meta.album_marquee.marquee_step();
        render();
    }
}

void display_large(const char *text)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(64 - (FONT_WIDTH * 2) * strlen(text), 16 - (FONT_HEIGHT * 2) / 2);
    display.println(text);
    display.display();
}

void display_small(const char *text)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(64 - FONT_WIDTH * strlen(text), 16 - FONT_HEIGHT / 2);
    display.println(text);
    display.display();
}

void setup_display()
{
    oledI2C.begin(18, 19);
    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        display.setTextWrap(false);

        // splash screen
        display_large("miatamoto");

        xTaskCreate(task_shift_text, "shift_text", 2048, NULL, 1, NULL);
    }
    else
    {
        DEBUG_PRINTLN(F("SSD1306 allocation failed"));
    }
}
#endif