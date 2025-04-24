#include <Arduino.h>

#include <globals.h>

bool media_enabled = true;
bool media_paused = true;
uint8_t esp_mac[6] = {0};

uint8_t global_marquee_clock = 0;
struct AVRCMetadata meta(&global_marquee_clock);
