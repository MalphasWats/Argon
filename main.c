#include <avr/io.h>

#include "main.h"

dword t = 0;
dword btn_timer = 0;

Mob mobs[MAX_MOBS];

byte buttons;

int main (void) 
{    
    initialise();
    
    byte debounce = 0;
    bool button_down = FALSE;
    
    for(ever)
    {
        t = millis();
        
        for (word i=0 ; i<(SCREEN_WIDTH*SCREEN_ROWS) ; i++)
            buffer[i] = TITLE[i];
        
        draw();
        
        buttons = ~PINC;
        if(buttons == _A && !button_down)
        {
            button_down = TRUE;
            debounce = t+10;
        }
        
        if (buttons != _A && button_down && debounce <= t)
        {
            button_down = FALSE;
            argon();
        }
    }
}

void argon(void)
{
    
    mobs[0] = (Mob){
        .vx=0,
        .vy=0,
        .sprite = (Sprite){.tile=&TILES[PLAYER_SHIP], .mask=&MASKS[PLAYER_SHIP], .x=2, .y=32-4,},
        .dead=FALSE,
    };
    
    Mob *player = &mobs[0];
    
    for(ever)
    {
        t = millis();
        
        buttons = ~PINC;
        if (btn_timer <= t)
        {
            if ( buttons & _A )
            {
                click();
            }
            
            if ( buttons & _UP )
            {
                player->sprite.y -= PLAYER_SPEED;
                if (player->sprite.y < 0)
                    player->sprite.y += PLAYER_SPEED;
            }
            if ( buttons & _DOWN )
            {
                player->sprite.y += PLAYER_SPEED;
                if (player->sprite.y > SCREEN_HEIGHT-8)
                    player->sprite.y -= PLAYER_SPEED;
            }
            if ( buttons & _LEFT )
            {
                player->sprite.x -= PLAYER_SPEED;
                if (player->sprite.x < 0)
                    player->sprite.x += PLAYER_SPEED;
            }
            if ( buttons & _RIGHT )
            {
                player->sprite.x += PLAYER_SPEED;
                if (player->sprite.x > SCREEN_WIDTH-8)
                    player->sprite.x -= PLAYER_SPEED;
            }
            
            //TODO: lazy
            btn_timer = t+20;
        }
        
        clear_buffer();
        
        for (byte i=0 ; i<MAX_MOBS ; i++)
        {
            if (!mobs[i].dead)
                draw_sprite(&mobs[i].sprite);
        }
            
        draw();
    }
}
