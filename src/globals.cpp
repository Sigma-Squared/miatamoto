#include <Arduino.h>

#include <globals.h>

Observable<bool> media_enabled = Observable<bool>(true);
bool media_paused = false;
uint8_t esp_mac[6] = {0};

uint8_t global_marquee_clock = 0;
struct AVRCMetadata meta(&global_marquee_clock);
