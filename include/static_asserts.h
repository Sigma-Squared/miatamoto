#include <Arduino.h>
#include <globals.h>

// ASSERT NO PINS ARE DUPLICATED

// 1) pack everything into one constexpr array:
constexpr std::array<uint8_t,
                     /*4 A2DP*/ 4 + /*2 disp*/ 2 + /*2 MOSFET*/ 2 + /*5 btns*/ 5>
    all_pins = {
        I2S_BCK, I2S_WS_LRCK, I2S_DATA_DIN, I2S_MCK,
        DISPLAY_SDA, DISPLAY_SCL,
        MEDIA_ENABLED_PIN_BAR, LIGHTS_ENABLED_PIN,
        BUTTON_PINS[0], BUTTON_PINS[1], BUTTON_PINS[2],
        BUTTON_PINS[3], BUTTON_PINS[4]};

// 2) constexpr helper to detect duplicates via a bit-mask
template <size_t N>
constexpr bool pins_are_unique(const std::array<uint8_t, N> &arr)
{
    uint64_t seen = 0;
    for (auto pin : arr)
    {
        uint64_t bit = (uint64_t{1} << pin);
        if (seen & bit) // already saw this pin?
            return false;
        seen |= bit;
    }
    return true;
}

// 3) static_assert fires at compile-time if any duplicates exist
static_assert(pins_are_unique(all_pins),
              "[ERROR] Duplicate GPIO pin use detected");