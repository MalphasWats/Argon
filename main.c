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
byte delta= 0;

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
    
    //byte map_dirty = TRUE;
    
    sprite sprites[MAX_SPRITES] = {
        {.glyph=PLAYER_SHIP, .x=2, .y=2*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=0*8},
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
        if (btn_timer == 0)
        {
            if (btn_val >= _UP-BTN_THRESHOLD && btn_val <= _UP+BTN_THRESHOLD)
            {
                //click();
                
                sprites[0].y -= SPEED;
                if (sprites[0].y > 250)
                    sprites[0].y = 0;
                
                //map_dirty = TRUE;
                
                btn_timer = t;
            }
            else if(btn_val >= _DOWN-BTN_THRESHOLD && btn_val <= _DOWN+BTN_THRESHOLD)
            {
                //click();
                
                sprites[0].y += SPEED;
                if (sprites[0].y > HEIGHT-8)
                    sprites[0].y = HEIGHT-8;
                
                //map_dirty = TRUE;
                
                btn_timer = t;
            }
            else if(btn_val >= _LEFT-BTN_THRESHOLD && btn_val <= _LEFT+BTN_THRESHOLD)
            {
                //click();
                
                sprites[0].x -= SPEED;
                if (sprites[0].x > 250)
                    sprites[0].x = 0;
                
                //map_dirty = TRUE;
                
                btn_timer = t;
            }
            else if(btn_val >= _RIGHT-BTN_THRESHOLD && btn_val <= _RIGHT+BTN_THRESHOLD)
            {
                //click();
                
                sprites[0].x += SPEED;
                if (sprites[0].x > WIDTH-8)
                    sprites[0].x = WIDTH-8;
                
                //map_dirty = TRUE;
                
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
        
        //if (map_dirty)
        //{
            set_display_col_row(0, 0);
            byte buffer;
            byte digits[4];
            byte fps = 1000 / delta;
            for (byte d=0 ; d<4 ; d++)
            {
                digits[3-d] = ((fps % 10)+1)*8;
                fps = fps / 10;
            }
            for (byte row=0 ; row<SCREEN_ROWS ; row++)
            {
                //set_display_col_row(0, row);
                
                
                for (byte x=0 ; x<WIDTH ; x++)
                {
                    byte col = x/8;
                    byte offset = x%8;
                    buffer = GLYPHS[MAP[ SCREEN_COLUMNS * row + col ]*8 + offset];
                    
                    for(byte e=0 ; e<MAX_SPRITES ; e++)
                    {
                        byte r = sprites[e].y/8;
                        byte r_offset = sprites[e].y%8;
                        if (x >= sprites[e].x && x < sprites[e].x+8)
                        {
                            if (row == r)
                            {
                                buffer |= (GLYPHS[sprites[e].glyph*8+(x-sprites[e].x)] >> r_offset);
                            }
                            else if (row == r+1 && r_offset > 0)
                            {
                                buffer |= (GLYPHS[sprites[e].glyph*8+(x-sprites[e].x)] << (8-r_offset));
                            }
                        }
                    }
                    
                    if (row == 7 && col < 4)
                    {
                        buffer = GLYPHS[digits[col]+offset];
                    }
                    
                    shift_out_byte(buffer);
                }
            }
            
            //map_dirty = FALSE;
        //}
        
        /*byte row = player.y / 8;
        byte shift = player.y % 8;
        
        if (shift)
        {
            byte glyph[8];
            for(byte i=0 ; i<8 ; i++)
            {
                //TODO: Needs to combine with tile beneath
                glyph[i] = GLYPHS[player.glyph*8+i] >> shift;
            }
            
            PORTB &= ~(1 << DC);                    // COMMAND
    
            send_command(0xB0 + row);               // PAGEADDR
            send_command((player.x) & 0x0F);           // Column start address (0 = reset)
            send_command(0x10 | ((player.x) >> 4));    // LOW COL ADDR
            
            PORTB |= 1 << DC;                       // DATA
            
            shift_out_block(&glyph[0], FALSE);
            
            for(byte i=0 ; i<8 ; i++)
            {
                //TODO: Needs to combine with tile beneath
                glyph[i] = GLYPHS[player.glyph*8+i] << (8-shift);
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
        }*/
        
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
