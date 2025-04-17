#ifndef A2DPSINK_H
#define A2DPSINK_H

#include <DEBUG.h>
#include <globals.h>

#define ENABLE_PROGRESS_BAR 0

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

void on_connection_state_changed(esp_a2d_connection_state_t state, void *obj)
{
    DEBUG_PRINTF("A2DP connection state: %s\n", a2dp_sink.to_str(state));
    switch (state)
    {
    case ESP_A2D_CONNECTION_STATE_CONNECTING:
        display_large("connecting");
        break;
    case ESP_A2D_CONNECTION_STATE_CONNECTED:
        char display_text[10] = {0};
        auto peer_name = a2dp_sink.get_peer_name();
        if (strlen(peer_name) > 0)
        {
            DEBUG_PRINTF("Connected to peer: %s", peer_name);
            snprintf(display_text, sizeof(display_text), ">%s", peer_name);
        }
        else
        {
            auto peer_addr = a2dp_sink.get_current_peer_address();
            auto peer_addr_str = a2dp_sink.to_str(*peer_addr);
            DEBUG_PRINTF("Connected to peer: %s", peer_addr_str);
            snprintf(display_text, sizeof(display_text), ">%s", peer_addr_str);
        }
        display_large(display_text);
        a2dp_sink.play();
        break;
    }
}

void setup_a2dpsink()
{
    auto cfg = i2s.defaultConfig();
    cfg.pin_bck = 14;
    cfg.pin_ws = 15;
    cfg.pin_data = 22;
    i2s.begin(cfg);
    a2dp_sink.set_auto_reconnect(true, 10);
    a2dp_sink.start("miatamoto", true);
    a2dp_sink.set_volume(127);
    a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM
#if ENABLE_PROGRESS_BAR
                                               | ESP_AVRC_MD_ATTR_PLAYING_TIME
#endif
    );
    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
    a2dp_sink.set_on_connection_state_changed(on_connection_state_changed);
    // a2dp_sink.set_on_audio_state_changed();
    // a2dp_sink.get_audio_state();
    // a2dp_sink.get_current_peer_address();
    // a2dp_sink.get_peer_name
    DEBUG_PRINTLN("Started A2DP sink.");
}

#endif
