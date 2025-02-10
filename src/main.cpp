#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
  Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);
}

void setup()
{
  Serial.begin(115200);
  auto cfg = i2s.defaultConfig();
  cfg.pin_bck = 14;
  cfg.pin_ws = 15;
  cfg.pin_data = 22;
  i2s.begin(cfg);

  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);

  a2dp_sink.start("miatamoto");
  Serial.print("Started A2DP sink.");
}

void loop()
{
}