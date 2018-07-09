#include <avr/io.h>

#include "main.h"

word t = 0;
word btn_timer = 0;

typedef struct sprite {
    byte glyph;
    byte x;
    byte y;
    byte row;
    byte y_offset;
} sprite;

word delta_sum = 0;
word delta_count = 0;
byte delta = 0;

byte HUD[4*8];
byte fps=0;

byte buffer[128];

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
        {.glyph=0, .x=0, .y=0},
        {.glyph=0, .x=0, .y=0},
        {.glyph=0, .x=0, .y=0},
        {.glyph=0, .x=0, .y=0},
        {.glyph=0, .x=0, .y=0},
        {.glyph=0, .x=0, .y=0},
        {.glyph=0, .x=0, .y=0},
            
        {.glyph=ENEMY_SHIP, .x=12*8, .y=0*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=1*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=2*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=3*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=4*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=5*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=6*8},
        {.glyph=ENEMY_SHIP, .x=12*8, .y=7*8},
        
        {.glyph=PLAYER_SHIP, .x=2, .y=2*8},
    };
    
    sprite *player = &sprites[MAX_SPRITES-1];
    
    for(ever)
    {
        t = millis();
        
        word btn_val = read_buttons();
        
        // TODO: Need to map values with A held down
        if (btn_val >= _UP-BTN_THRESHOLD && btn_val <= _UP+BTN_THRESHOLD)
        {
            player->y -= SPEED;
            if (player->y > 255-SPEED-1)
                player->y = 0;
            player->row = player->y/8;
            player->y_offset = player->y%8;
        }
        else if(btn_val >= _DOWN-BTN_THRESHOLD && btn_val <= _DOWN+BTN_THRESHOLD)
        {
            player->y += SPEED;
            if (player->y > SCREEN_HEIGHT-8)
                player->y = SCREEN_HEIGHT-8;
            player->row = player->y/8;
            player->y_offset = player->y%8;
        }
        else if(btn_val >= _LEFT-BTN_THRESHOLD && btn_val <= _LEFT+BTN_THRESHOLD)
        {
            player->x -= SPEED;
            if (player->x > 255-SPEED-1)
                player->x = 0;
        }
        else if(btn_val >= _RIGHT-BTN_THRESHOLD && btn_val <= _RIGHT+BTN_THRESHOLD)
        {
            player->x += SPEED;
            if (player->x > SCREEN_WIDTH-8)
                player->x = SCREEN_WIDTH-8;
        }
        if (btn_timer == 0)
        {
            if(btn_val >= _A-BTN_THRESHOLD && btn_val <= _A+BTN_THRESHOLD)
            {
                click();
                for (byte s=0 ; s < 4 ; s++)
                {
                    if (sprites[s].glyph == 0 && player->x < 120)
                    {
                        sprites[s].glyph = PLASMA_BOLT;
                        sprites[s].x = player->x+8;
                        sprites[s].y = player->y;
                        
                        break;
                    }
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
        
        set_display_col_row(0, 0);
        for (byte row=0 ; row<SCREEN_ROWS ; row++)
        {
            for (byte x=0 ; x<SCREEN_WIDTH ; x++)
            {
                byte col = x/8;
                byte offset = x%8;
                buffer[x] = GLYPHS[MAP[ SCREEN_COLUMNS * row + col ]*8 + offset];
            }
            
            for(byte s=0 ; s<MAX_SPRITES ; s++)
            {
                if (row == sprites[s].row)
                {
                    buffer[sprites[s].x+0] |= GLYPHS[sprites[s].glyph*8+0] >> sprites[s].y_offset;
                    buffer[sprites[s].x+1] |= GLYPHS[sprites[s].glyph*8+1] >> sprites[s].y_offset;
                    buffer[sprites[s].x+2] |= GLYPHS[sprites[s].glyph*8+2] >> sprites[s].y_offset;
                    buffer[sprites[s].x+3] |= GLYPHS[sprites[s].glyph*8+3] >> sprites[s].y_offset;
                    buffer[sprites[s].x+4] |= GLYPHS[sprites[s].glyph*8+4] >> sprites[s].y_offset;
                    buffer[sprites[s].x+5] |= GLYPHS[sprites[s].glyph*8+5] >> sprites[s].y_offset;
                    buffer[sprites[s].x+6] |= GLYPHS[sprites[s].glyph*8+6] >> sprites[s].y_offset;
                    buffer[sprites[s].x+7] |= GLYPHS[sprites[s].glyph*8+7] >> sprites[s].y_offset;
                }
                else if (row == sprites[s].row+1 && sprites[s].y_offset > 0)
                {
                    buffer[sprites[s].x+0] |= GLYPHS[sprites[s].glyph*8+0] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+1] |= GLYPHS[sprites[s].glyph*8+1] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+2] |= GLYPHS[sprites[s].glyph*8+2] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+3] |= GLYPHS[sprites[s].glyph*8+3] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+4] |= GLYPHS[sprites[s].glyph*8+4] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+5] |= GLYPHS[sprites[s].glyph*8+5] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+6] |= GLYPHS[sprites[s].glyph*8+6] << (8-sprites[s].y_offset);
                    buffer[sprites[s].x+7] |= GLYPHS[sprites[s].glyph*8+7] << (8-sprites[s].y_offset);
                }
            }
            
            /* Display HUD */
            if (row == 7)
            {
                for (byte i=0 ; i<4*8 ; i++)
                    buffer[i] = HUD[i];
            }
            
            for (byte x=0 ; x<SCREEN_WIDTH ; x++)
                shift_out_byte(buffer[x]);
        }
        
        /* Update Sprites */
        for(byte s=0 ; s<MAX_SPRITES ; s++)
        {
            // Collision detection
            for(byte _s=0 ; _s<MAX_SPRITES ; _s++)
            {
                // Don't check self! Also, sprites can't collide with other sprites of same type
                if (s != _s && sprites[s].glyph != sprites[_s].glyph)
                {
                    if (sprites[s].x >= sprites[_s].x && sprites[s].x < sprites[_s].x+8 &&
                        sprites[s].y >= sprites[_s].y && sprites[s].y < sprites[_s].y+8)
                    {
                        sprites[s].glyph=0;
                        sprites[s].x = 0;
                        sprites[s].y = 0;
                        
                        sprites[_s].glyph=0;
                        sprites[_s].x = 0;
                        sprites[_s].y = 0;
                    }
                }
            }
            
            if (sprites[s].glyph == PLASMA_BOLT)
            {
                sprites[s].x += 6;
                if (sprites[s].x >= 120)
                {
                    sprites[s].glyph = 0;
                    sprites[s].x = 0;
                    sprites[s].y = 0;
                }
            }
            else if (sprites[s].glyph == ENEMY_SHIP)
            {
                sprites[s].x -= 2;
                if (sprites[s].x <= 0)
                {
                    //TODO: Do something cleverer here!
                    sprites[s].glyph=0;
                    sprites[s].x = 0;
                    sprites[s].y = 0;
                }
            }
        }
        
        /* Update Framerate (DEBUGGING) */
        delta_sum += millis() - t;
        delta_count += 1;
        if (delta_count == 25) // Update every 25 frames
        {
            delta = delta_sum / delta_count;
            if (delta != 0)
                fps = 1000 / delta;
            
            delta_sum = 0;
            delta_count = 0;
            byte digit = 0;
            for (byte d=0 ; d<4 ; d++)
            {
                digit = ((fps % 10)+1)*8;
                for(byte i=0 ; i<8 ; i++)
                    HUD[(d*8)+i] = GLYPHS[digit+i];
                fps = fps / 10;
            }
        }
    }
}
