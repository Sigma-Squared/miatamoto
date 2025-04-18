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
}

void loop()
{
    button_loop();
}