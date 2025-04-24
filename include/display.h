#ifndef DISPLAY_H
#define DISPLAY_H

enum display_state_t
{
    DISPLAY_NONE,
    DISPLAY_ANIMATION,
    DISPLAY_PLAYING,
    DISPLAY_AMPOFF
};
enum animation_state_t
{
    ANIMATION_BT_CONNECTING_DISCONNECTING,
    ANIMATION_BT_CONNECTED,
    ANIMATION_BT_DISCONNECTED,
};
void display_setstate(display_state_t state);
void display_animation_setstate(animation_state_t state);
void setup_display();

#endif