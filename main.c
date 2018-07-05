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

word delta_sum = 0;
word delta_count = 0;
byte delta = 0;

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
    
    sprite sprites[MAX_SPRITES] = {
        {.glyph=PLAYER_SHIP, .x=2, .y=2*8},
        {.glyph=0, .x=0, .y=0},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=1*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=2*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=3*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=4*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=5*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=6*8},
    };
    
    for(ever)
    {
        t = millis();
        
        word btn_val = read_buttons();
        
        // TODO: Need to map values with A held down
        if (btn_val >= _UP-BTN_THRESHOLD && btn_val <= _UP+BTN_THRESHOLD)
        {
            sprites[0].y -= SPEED;
            if (sprites[0].y > 250)
                sprites[0].y = 0;
        }
        else if(btn_val >= _DOWN-BTN_THRESHOLD && btn_val <= _DOWN+BTN_THRESHOLD)
        {
            sprites[0].y += SPEED;
            if (sprites[0].y > HEIGHT-8)
                sprites[0].y = HEIGHT-8;
        }
        else if(btn_val >= _LEFT-BTN_THRESHOLD && btn_val <= _LEFT+BTN_THRESHOLD)
        {
            sprites[0].x -= SPEED;
            if (sprites[0].x > 250)
                sprites[0].x = 0;
        }
        else if(btn_val >= _RIGHT-BTN_THRESHOLD && btn_val <= _RIGHT+BTN_THRESHOLD)
        {
            sprites[0].x += SPEED;
            if (sprites[0].x > WIDTH-8)
                sprites[0].x = WIDTH-8;
        }
        if (btn_timer == 0)
        {
            if(btn_val >= _A-BTN_THRESHOLD && btn_val <= _A+BTN_THRESHOLD)
            {
                click();
                if (sprites[1].glyph == 0)
                {
                    sprites[1].glyph = PLASMA_BOLT;
                    sprites[1].x = sprites[0].x+8;
                    sprites[1].y = sprites[0].y;
                }
                
                btn_timer = t+BTN_DELAY;
            }
            else if(btn_val >= _B-BTN_THRESHOLD && btn_val <= _B+BTN_THRESHOLD)
            {
                click();
                btn_timer = t+BTN_DELAY;
            }
            else if(btn_val >= _C-BTN_THRESHOLD && btn_val <= _C+BTN_THRESHOLD)
            {
                click();
                btn_timer = t+BTN_DELAY;
            }
        }
        
        if (t > btn_timer)
            btn_timer = 0;
            
        byte buffer[128];
        byte digits[4];
        byte fps = 1000 / delta;
        
        //set_display_col_row(0, 0);
        
        for (byte d=0 ; d<4 ; d++)
        {
            digits[3-d] = ((fps % 10)+1)*8;
            fps = fps / 10;
        }
        
        for (byte row=0 ; row<SCREEN_ROWS ; row++)
        {
            for (byte x=0 ; x<WIDTH ; x++)
            {
                byte col = x/8;
                byte offset = x%8;
                buffer[x] = GLYPHS[MAP[ SCREEN_COLUMNS * row + col ]*8 + offset];
            }
            
            for(byte s=0 ; s<MAX_SPRITES ; s++)
            {
                byte r = sprites[s].y/8;
                byte r_offset = sprites[s].y%8;
                
                if (row == r)
                {
                    for(byte i=0 ; i<8 ; i++)
                        buffer[sprites[s].x+i] |= GLYPHS[sprites[s].glyph*8+i] >> r_offset;
                }
                else if (row == r+1 && r_offset > 0)
                {
                    for(byte i=0 ; i<8 ; i++)
                        buffer[sprites[s].x+i] |= GLYPHS[sprites[s].glyph*8+i] << (8-r_offset);
                }
            }
                
            if (row == 7)
            {
                for (byte d=0 ; d<4*8 ; d++)
                    buffer[d] = GLYPHS[digits[d/8]+d%8];
            }
            
            for (byte x=0 ; x<WIDTH ; x++)
                shift_out_byte(buffer[x]);
        }
        
        /* Update Sprites */
        for(byte s=1 ; s<MAX_SPRITES ; s++)
        {
            if (sprites[s].glyph == PLASMA_BOLT)
            {
                sprites[s].x += 6;
                if (sprites[s].x > 128)
                {
                    sprites[s].glyph = 0;
                    sprites[s].x = 0;
                    sprites[s].y = 0;
                }
            }
        }
        
        delta_sum += millis() - t;
        delta_count += 1;
        if (delta_count == 100)
        {
            if (delta_sum / delta_count != 0)
                delta = delta_sum / delta_count;
            
            delta_sum = 0;
            delta_count = 0;
        }
    }
}
