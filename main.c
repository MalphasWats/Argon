#include "main.h"
#include "argon.h"

int main (void)
{
    initialise();

    uint32_t t = 0;

    uint8_t buttons;

    uint8_t debounce = 0;
    bool button_was_down = FALSE;

    for(ever)
    {
        t = millis();

        draw_image(&TITLE, 0, 0);

        draw();

        buttons = read_buttons();
        if(buttons == BTN_A && !button_was_down)
        {
            button_was_down = TRUE;
            debounce = t+10;
        }

        if (buttons != BTN_A && button_was_down && debounce <= t)
        {
            button_was_down = FALSE;
            argon();
        }
    }
}
