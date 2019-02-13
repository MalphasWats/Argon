#include "argon.h"

Mob mobs[MAX_MOBS];

void argon(void)
{
    dword t = 0;
    dword btn_timer = 0;
    
    byte buttons;

    mobs[0] = (Mob){
        .vx=0,
        .vy=0,
        .sprite = (Sprite){.tile=&TILES[PLAYER_SHIP], .mask=&MASKS[PLAYER_SHIP], .x=2, .y=32-4,},
        .active=TRUE,
    };
    
    Mob *player = &mobs[0];
    
    for(ever)
    {
        t = millis();
        
        buttons = read_buttons();
        if (btn_timer <= t)
        {
            if ( buttons & BTN_A )
            {
                click();
            }
            
            if ( buttons & BTN_UP )
            {
                player->sprite.y -= PLAYER_SPEED;
                if (player->sprite.y < 0)
                    player->sprite.y += PLAYER_SPEED;
            }
            if ( buttons & BTN_DOWN )
            {
                player->sprite.y += PLAYER_SPEED;
                if (player->sprite.y > SCREEN_HEIGHT-8)
                    player->sprite.y -= PLAYER_SPEED;
            }
            if ( buttons & BTN_LEFT )
            {
                player->sprite.x -= PLAYER_SPEED;
                if (player->sprite.x < 0)
                    player->sprite.x += PLAYER_SPEED;
            }
            if ( buttons & BTN_RIGHT )
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
            if (mobs[i].active)
                draw_sprite(&mobs[i].sprite);
        }
            
        draw();
    }
}
