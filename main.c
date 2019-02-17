#include "main.h"
#include "argon.h"

int main (void) 
{    
    initialise();
    
    dword t = 0;

    byte buttons;
    
    byte debounce = 0;
    bool button_was_down = FALSE;
    
    for(ever)
    {
        t = millis();
        
        for (word i=0 ; i<(SCREEN_WIDTH*SCREEN_ROWS) ; i++)
            buffer[i] = TITLE[i];
        
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

