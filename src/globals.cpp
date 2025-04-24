#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#include <marquee.h>
#include <globals.h>

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

bool splash = true;
bool media_enabled = true;
bool media_paused = true;
uint8_t esp_mac[6] = {0};

uint8_t global_marquee_clock = 0;
struct AVRCMetadata meta(&global_marquee_clock);
