#ifndef DISPLAY_H
#define DISPLAY_H

enum display_state_t
{
    DISPLAY_NONE,
    DISPLAY_IDLE,
    DISPLAY_PLAYING,
    DISPLAY_AMPOFF
};
void display_setstate(display_state_t state);
void setup_display();

#endif