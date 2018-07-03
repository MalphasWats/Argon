#include <avr/io.h>

#include "main.h"
#include "oled.h"
#include "beep.h"

word t = 0;
word btn_timer = 0;

typedef struct sprite {
    byte glyph;
    byte x;
    byte y;
} sprite;

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
    
    sprite player = {.glyph=PLAYER_SHIP, .x=2, .y=2*8};
    
    for(ever)
    {
        t = millis();
        
        word btn_val = read_buttons();
        if (btn_timer == 0)
        {
            if (btn_val >= _UP-BTN_THRESHOLD && btn_val <= _UP+BTN_THRESHOLD)
            {
                click();
                
                player.y -= 1;
                if (player.y > 250)
                    player.y = 0;
                
                map_dirty = TRUE;
                
                btn_timer = t;
            }
            else if(btn_val >= _DOWN-BTN_THRESHOLD && btn_val <= _DOWN+BTN_THRESHOLD)
            {
                click();
                
                player.y += 1;
                if (player.y > HEIGHT-8)
                    player.y = HEIGHT-8;
                
                map_dirty = TRUE;
                
                btn_timer = t;
            }
            else if(btn_val >= _LEFT-BTN_THRESHOLD && btn_val <= _LEFT+BTN_THRESHOLD)
            {
                click();
                
                player.x -= 1;
                if (player.x > 250)
                    player.x = 0;
                
                map_dirty = TRUE;
                
                btn_timer = t;
            }
            else if(btn_val >= _RIGHT-BTN_THRESHOLD && btn_val <= _RIGHT+BTN_THRESHOLD)
            {
                click();
                
                player.x += 1;
                if (player.x > WIDTH)
                    player.x = WIDTH-8;
                
                map_dirty = TRUE;
                
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
        
        byte row = player.y / 8;
        byte shift = player.y % 8;
        
        if (shift)
        {
            byte glyph[8];
            for(byte i ; i<8 ; i++)
            {
                //TODO: Needs to combine with tile beneath
                glyph[i] = GLYPHS[player.glyph*8] >> shift;
            }
            
            PORTB &= ~(1 << DC);                    // COMMAND
    
            send_command(0xB0 + row);               // PAGEADDR
            send_command((player.x) & 0x0F);           // Column start address (0 = reset)
            send_command(0x10 | ((player.x) >> 4));    // LOW COL ADDR
            
            PORTB |= 1 << DC;                       // DATA
            
            shift_out_block(&glyph[0], FALSE);
            
            for(byte i ; i<8 ; i++)
            {
                //TODO: Needs to combine with tile beneath
                glyph[i] = GLYPHS[player.glyph*8] << 8-shift;
            }
            
            PORTB &= ~(1 << DC);                    // COMMAND
    
            send_command(0xB0 + row+1);               // PAGEADDR
            send_command((player.x) & 0x0F);           // Column start address (0 = reset)
            send_command(0x10 | ((player.x) >> 4));    // LOW COL ADDR
            
            PORTB |= 1 << DC;                       // DATA
            
            shift_out_block(&glyph[0], FALSE);
        }
        else
        {
            PORTB &= ~(1 << DC);                    // COMMAND
    
            send_command(0xB0 + row);               // PAGEADDR
            send_command((player.x) & 0x0F);           // Column start address (0 = reset)
            send_command(0x10 | ((player.x) >> 4));    // LOW COL ADDR
            
            PORTB |= 1 << DC;                       // DATA
            
            shift_out_block(&GLYPHS[player.glyph*8], FALSE);
        }
    }
}
