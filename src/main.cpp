#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CHARLIM 11
#define DATALIM 100
#define EDGE_STALL 5
#define FONT_WIDTH 6
#define FONT_HEIGHT 8
#define ENABLE_PROGRESS_BAR 0
#define DEBUG 0

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

TwoWire oledI2C = TwoWire(1);
Adafruit_SSD1306 display(128, 32, &oledI2C, -1);
struct AVRCMetadata
{
  char title[DATALIM];
  char artist[DATALIM];
  char album[DATALIM];
  uint32_t playtime_ms;
  uint32_t play_start;
  uint8_t title_len;
  uint8_t artist_len;
  uint8_t album_len;
  char rendered_title[CHARLIM + 1];
  char rendered_artist[CHARLIM * 2 + 1];
  char rendered_album[CHARLIM * 2 + 1];
  uint8_t marquee_offset;
};

bool splash = true;
static struct AVRCMetadata meta;

uint8_t clamp(int8_t val, uint8_t min, uint8_t max)
{
  if (val < min)
    return min;
  if (val > max)
    return max;
  return val;
}

void render()
{
  if (splash)
    return;
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(meta.rendered_title);
  display.setTextSize(1);
  display.println(meta.rendered_artist);
  display.println(meta.rendered_album);
  if (meta.playtime_ms > 0)
  {
    uint32_t elapsed = millis() - meta.play_start;
    uint32_t progress = (elapsed * 128) / meta.playtime_ms;
    display.fillRect(0, 31, progress, 2, SSD1306_WHITE);
  }
  display.display();
}

void wrap_text(char *out, const char *in, uint8_t in_len, uint8_t cutoff)
{
  if (in_len > cutoff)
  {
    uint8_t max_offset = in_len - cutoff;
    uint8_t wrap = max_offset + 2 * EDGE_STALL + 1;
    strncpy(out, in + clamp((meta.marquee_offset % wrap) - EDGE_STALL, 0, max_offset + 1), cutoff);
    out[cutoff] = '\0';
  }
  else
  {
    strcpy(out, in);
  }
}

void metadata_to_render()
{
  wrap_text(meta.rendered_title, meta.title, meta.title_len, CHARLIM);
  wrap_text(meta.rendered_album, meta.album, meta.album_len, CHARLIM * 2);
  wrap_text(meta.rendered_artist, meta.artist, meta.artist_len, CHARLIM * 2);
}

void avrc_metadata_callback(uint8_t type, const uint8_t *payload)
{
#if (DEBUG)
  Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", type, payload);
#endif
  const char *payload_str = (const char *)payload;
  splash = false;

  switch (type)
  {
  case ESP_AVRC_MD_ATTR_TITLE:
    meta.title_len = strlen(payload_str);
    strcpy(meta.title, payload_str);
    break;
  case ESP_AVRC_MD_ATTR_ARTIST:
    meta.artist_len = strlen(payload_str);
    strcpy(meta.artist, payload_str);
    break;
  case ESP_AVRC_MD_ATTR_ALBUM:
    meta.album_len = strlen(payload_str);
    strcpy(meta.album, payload_str);
    break;
#if ENABLE_PROGRESS_BAR
  case ESP_AVRC_MD_ATTR_PLAYING_TIME:
    meta.playtime_ms = String(payload_str).toInt();
    meta.play_start = millis();
    break;
#endif
  }
  meta.marquee_offset = 0;
  metadata_to_render();
  render();
}

void task_shift_text(void *_)
{
  for (;;)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    meta.marquee_offset++;
    metadata_to_render();
    render();
  }
}

void setup()
{
#if (DEBUG)
  Serial.begin(115200);
#endif
  auto cfg = i2s.defaultConfig();
  cfg.pin_bck = 14;
  cfg.pin_ws = 15;
  cfg.pin_data = 22;
  i2s.begin(cfg);

  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_PLAYING_TIME);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);

  a2dp_sink.start("miatamoto");
#if (DEBUG)
  Serial.print("Started A2DP sink.");
#endif

  oledI2C.begin(18, 19);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
#if (DEBUG)
    Serial.println(F("SSD1306 allocation failed"));
#endif
    for (;;)
      ; // Don't proceed, loop forever
  }
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

void loop()
{
}