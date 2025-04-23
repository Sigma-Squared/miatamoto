#ifndef A2DPSINK_H
#define A2DPSINK_H

#include <DEBUG.h>
#include <globals.h>
#include <driver/gpio.h>

constexpr bool ENABLE_PROGRESS_BAR = false;
constexpr bool USE_MCK = false; // set to true if using MCK pin for I2S

void avrc_metadata_callback(uint8_t type, const uint8_t *payload)
{
    DEBUG_PRINTF("AVRC metadata rsp: attribute id 0x%x, %s\n", type, payload);
    const char *payload_str = (const char *)payload;
    if (strlen(payload_str) == 0)
        return;
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
    render();
}

void on_connection_state_changed(esp_a2d_connection_state_t state, void *obj)
{
    DEBUG_PRINTF("A2DP connection state: %s\n", a2dp_sink.to_str(state));
    switch (state)
    {
    case ESP_A2D_CONNECTION_STATE_CONNECTING:
        display_small("connecting...");
        break;
    case ESP_A2D_CONNECTION_STATE_CONNECTED:
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
        display_small(display_text);
        a2dp_sink.play();
        break;
    }
}

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
    char bt_name[13] = {0};
    sprintf(bt_name, "miatamoto %02x", esp_mac[5]);
    a2dp_sink.start(bt_name, true);
    a2dp_sink.set_volume(127);
    a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | (ENABLE_PROGRESS_BAR ? ESP_AVRC_MD_ATTR_PLAYING_TIME : 0));
    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
    a2dp_sink.set_on_connection_state_changed(on_connection_state_changed);
    DEBUG_PRINTLN("Started A2DP sink.");
}

#endif
