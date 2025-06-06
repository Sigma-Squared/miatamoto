#include <Arduino.h>

#include <DEBUG.h>
#include <a2dpsink.h>
#include <globals.h>
#include <display.h>

constexpr bool ENABLE_PROGRESS_BAR = false;
constexpr bool USE_MCK = false;           // set to true if using MCK pin for I2S
constexpr long ONSTART_PLAY_DELAY = 1500; // delay before starting playback after connection

void avrc_metadata_callback(uint8_t type, const uint8_t *payload)
{
    DEBUG_PRINTF("AVRC metadata rsp: attribute id 0x%x, %s\n", type, payload);
    const char *payload_str = (const char *)payload;
    if (strlen(payload_str) == 0)
    {
        display_setstate(DISPLAY_ANIMATION);
        return;
    }

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
    case ESP_AVRC_MD_ATTR_PLAYING_TIME:
        if (!ENABLE_PROGRESS_BAR)
            break;
        meta.playtime_ms = String(payload_str).toInt();
        meta.play_start = millis();
        break;
    }
    global_marquee_clock = 0;
    meta.title_marquee.marquee_step();
    meta.artist_marquee.marquee_step();
    meta.album_marquee.marquee_step();
    display_setstate(DISPLAY_PLAYING);
}

void task_delay_play(void *_)
{
    vTaskDelay(ONSTART_PLAY_DELAY / portTICK_PERIOD_MS);
    a2dp_sink.play();
    media_paused = false;
    vTaskDelete(NULL);
}

void on_connection_state_changed(esp_a2d_connection_state_t state, void *obj)
{
    DEBUG_PRINTF("A2DP connection state: %s\n", a2dp_sink.to_str(state));
    switch (state)
    {
    case ESP_A2D_CONNECTION_STATE_CONNECTING:
    case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
        display_animation_setstate(ANIMATION_BT_CONNECTING_DISCONNECTING);
        break;
    case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
        display_setstate(DISPLAY_ANIMATION);
        display_animation_setstate(ANIMATION_BT_DISCONNECTED);
        break;
    case ESP_A2D_CONNECTION_STATE_CONNECTED:
        display_animation_setstate(ANIMATION_BT_CONNECTED);
        char display_text[20] = {0};
        auto peer_name = a2dp_sink.get_peer_name();
        if (strlen(peer_name) > 0)
        {
            DEBUG_PRINTF("Connected to peer: %s", peer_name);
            snprintf(display_text, 10, ">>%s", peer_name);
        }
        else
        {
            auto peer_addr = a2dp_sink.get_current_peer_address();
            auto peer_addr_str = a2dp_sink.to_str(*peer_addr);
            DEBUG_PRINTF("Connected to peer: %s", peer_addr_str);
            snprintf(display_text, 20, ">>%s", peer_addr_str);
        }
        xTaskCreate(task_delay_play, "play_task", 1024, NULL, 1, NULL);
        break;
    }
}

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup_a2dpsink()
{
    auto cfg = i2s.defaultConfig();
    cfg.pin_bck = I2S_BCK;
    cfg.pin_ws = I2S_WS_LRCK;
    cfg.pin_data = I2S_DATA_DIN;
    if (USE_MCK)
    {
        cfg.pin_mck = I2S_MCK;
        cfg.use_apll = true;
    }
    else
    {
        pinMode(I2S_MCK, OUTPUT);
        digitalWrite(I2S_MCK, LOW);
    }
    i2s.begin(cfg);

#if DEBUG
    a2dp_sink.set_sample_rate_callback([](uint16_t rate)
                                       { DEBUG_PRINTF("Sample rate: %d\n", rate); });
#endif
    a2dp_sink.set_auto_reconnect(true, 10);
    a2dp_sink.start("miatamoto", true);
    a2dp_sink.set_volume(127);
    a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | (ENABLE_PROGRESS_BAR ? ESP_AVRC_MD_ATTR_PLAYING_TIME : 0));
    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
    a2dp_sink.set_on_connection_state_changed(on_connection_state_changed);
    DEBUG_PRINTLN("Started A2DP sink.");
}
