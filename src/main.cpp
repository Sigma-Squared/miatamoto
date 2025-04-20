#define DEBUG true
#include <DEBUG.h>
#include <display.h>
#include <a2dpsink.h>
#include <buttons.h>

void setup()
{
    DEBUG_BEGIN(115200);

    setup_display();
    setup_a2dpsink();
    setup_buttons();

#if DEBUG
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    Serial.printf("ESP32 revision %d\n", chip_info.revision);
#endif
}

void loop()
{
    button_loop();
}