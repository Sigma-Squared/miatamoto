template <uint8_t CUTOFF, uint8_t EDGESTALL, uint8_t MAX = 128>
class MarqueeText
{
private:
    char text[MAX];
    uint8_t text_len;
    const uint8_t *const clock;

    static uint8_t clamp(int8_t val, uint8_t min, uint8_t max)
    {
        if (val < min)
            return min;
        if (val > max)
            return max;
        return val;
    }

public:
    char rendered_text[CUTOFF + 1];

    MarqueeText(const uint8_t *const clock) : text_len(0), clock(clock)
    {
        text[0] = '\0';
        rendered_text[0] = '\0';
    }
    void set(const char *new_text)
    {
        text_len = strlen(new_text);
        strcpy(text, new_text);
    }
    void marquee_step()
    {
        if (text_len > CUTOFF)
        {
            uint8_t max_offset = text_len - CUTOFF;
            uint8_t wrap = max_offset + 2 * EDGESTALL + 1;
            strncpy(rendered_text, text + clamp((*clock % wrap) - EDGESTALL, 0, max_offset + 1), CUTOFF);
            rendered_text[CUTOFF] = '\0';
        }
        else
        {
            strcpy(rendered_text, text);
        }
    }
};