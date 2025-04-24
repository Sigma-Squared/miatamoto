#include <DEBUG.h>
#include <display.h>
#include <a2dpsink.h>
#include <buttons.h>
#include <static_asserts.h>

void setup()
{
    DEBUG_BEGIN(115200);
    esp_efuse_mac_get_default(esp_mac);

#if DEBUG
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    Serial.printf("ESP32 chip info {\n  mac: %02x:%02x:%02x:%02x:%02x:%02x,\n  model: %d,\n  features: 0x%X,\n  cores: %d,\n  rev: %d,\n}\n",
                  esp_mac[0], esp_mac[1], esp_mac[2], esp_mac[3], esp_mac[4], esp_mac[5], chip_info.model, chip_info.features, chip_info.cores, chip_info.revision);
#endif

    setup_display();
    setup_a2dpsink();
    setup_buttons();
}

void loop()
{
    button_loop();
}