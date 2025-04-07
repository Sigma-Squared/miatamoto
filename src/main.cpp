#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <marquee.h>
#include <rotary.h>
#define DEBUG true
#include <DEBUG.h>

#define CHARLIM 11
#define DATALIM 100
#define EDGE_STALL 5
#define FONT_WIDTH 6
#define FONT_HEIGHT 8
#define ENABLE_PROGRESS_BAR 0

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

TwoWire oledI2C = TwoWire(1);
Adafruit_SSD1306 display(128, 32, &oledI2C, -1);

struct AVRCMetadata
{
    MarqueeText<CHARLIM, EDGE_STALL> title_marquee;
    MarqueeText<CHARLIM * 2, EDGE_STALL> artist_marquee;
    MarqueeText<CHARLIM * 2, EDGE_STALL> album_marquee;
    uint32_t playtime_ms;
    uint32_t play_start;
    AVRCMetadata(const uint8_t *const clock) : playtime_ms(0), play_start(0), title_marquee(clock), artist_marquee(clock), album_marquee(clock) {}
};

bool splash = true;
uint8_t global_marquee_clock = 0;
static struct AVRCMetadata meta(&global_marquee_clock);

void render()
{
    if (splash)
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

void avrc_metadata_callback(uint8_t type, const uint8_t *payload)
{
    DEBUG_PRINTF("AVRC metadata rsp: attribute id 0x%x, %s\n", type, payload);
    const char *payload_str = (const char *)payload;
    splash = false;

    switch (type)
    {
    case ESP_AVRC_MD_ATTR_TITLE:
        meta.title_marquee.set(payload_str);
        break;
    case ESP_AVRC_MD_ATTR_ARTIST:
        meta.artist_marquee.set(payload_str);
        break;
    case ESP_AVRC_MD_ATTR_ALBUM:
        meta.album_marquee.set(payload_str);
        break;
#if ENABLE_PROGRESS_BAR
    case ESP_AVRC_MD_ATTR_PLAYING_TIME:
        meta.playtime_ms = String(payload_str).toInt();
        meta.play_start = millis();
        break;
#endif
    }
    global_marquee_clock = 0;
    meta.title_marquee.marquee_step();
    meta.artist_marquee.marquee_step();
    meta.album_marquee.marquee_step();
    render();
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

void setup_display()
{
    oledI2C.begin(18, 19);
    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        display.setTextWrap(false);

        // splash screen
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(2);
        display.setCursor(64 - FONT_WIDTH * 9, 16 - FONT_HEIGHT / 2);
        display.println("miatamoto");
        display.display();

        xTaskCreate(task_shift_text, "shift_text", 2048, NULL, 1, NULL);
    }
    else
    {
        DEBUG_PRINTLN(F("SSD1306 allocation failed"));
    }
}

void setup_a2dpsink()
{
    auto cfg = i2s.defaultConfig();
    cfg.pin_bck = 14;
    cfg.pin_ws = 15;
    cfg.pin_data = 22;
    i2s.begin(cfg);

    a2dp_sink.start("miatamoto", true);
    a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM
#if ENABLE_PROGRESS_BAR
                                               | ESP_AVRC_MD_ATTR_PLAYING_TIME
#endif
    );
    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
    DEBUG_PRINTLN("Started A2DP sink.");
}

void setup()
{
    DEBUG_BEGIN(115200);

    setup_display();
    // setup_encoder();
    setup_a2dpsink();
}

void loop()
{
}