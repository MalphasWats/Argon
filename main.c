#include <avr/io.h>

#include "main.h"
#include "oled.h"
#include "beep.h"

word t = 0;
word btn_timer = 0;

int main (void) 
{    
    initialise();
    initialise_oled();
    clear_display();
    
    PORTB |= 1 << DC;           // DATA
    
    display_image(&LOGO[0], 3, 3, 10, 2);
    crap_beep(_A5, 140);
    delay_ms(5);
    crap_beep(_A8, 60);
    
    delay_ms(SPLASH_DELAY);
    
    byte map_dirty = TRUE;
    
    for(ever)
    {
        t = millis();
        
        word btn_val = read_buttons();
        if (btn_timer == 0)
        {
            if (btn_val >= _UP-BTN_THRESHOLD && btn_val <= _UP+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
            else if(btn_val >= _DOWN-BTN_THRESHOLD && btn_val <= _DOWN+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
            else if(btn_val >= _LEFT-BTN_THRESHOLD && btn_val <= _LEFT+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
            else if(btn_val >= _RIGHT-BTN_THRESHOLD && btn_val <= _RIGHT+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
            else if(btn_val >= _A-BTN_THRESHOLD && btn_val <= _A+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
            else if(btn_val >= _B-BTN_THRESHOLD && btn_val <= _B+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
            else if(btn_val >= _C-BTN_THRESHOLD && btn_val <= _C+BTN_THRESHOLD)
            {
                click();
                btn_timer = t;
            }
        }
        
        if (t - btn_timer >= BTN_DELAY)
            btn_timer = 0;
        
        if (map_dirty)
        {
            for (byte row=0 ; row<SCREEN_ROWS ; row++)
            {
                set_display_col_row(0, row);
                for (byte col=0 ; col<SCREEN_COLUMNS ; col++)
                {
                    shift_out_block(&GLYPHS[MAP[ SCREEN_COLUMNS * row + col ]*8], FALSE);
                }
            }
            
            map_dirty = FALSE;
        }
        
        
    }
}
