#include "argon.h"

Mob mobs[MAX_MOBS];

Mob stars[MAX_STARS];

void argon(void)
{
    dword t = 0;
    dword btn_timer = 0;
    
    dword star_timer = 0;
    
    byte buttons;

    mobs[0] = (Mob){
        .vx=0,
        .vy=0,
        .sprite = (Sprite){.tile=&TILES[PLAYER_SHIP], .mask=&MASKS[PLAYER_SHIP], .x=2, .y=32-4,},
        .active=TRUE,
    };
    
    Mob *player = &mobs[0];
    
    for(byte i=0 ; i<MAX_STARS ; i++)
    {
        byte star = rng() & 15;
        int vx = -1;
        if (star == 15)
            vx = -2;
            
        byte c = rng() & (SCREEN_COLUMNS-1);
        byte r = rng() & (SCREEN_ROWS-1);
            
        stars[i] = (Mob){
            .vx=vx,
            .vy=0,
            .sprite = (Sprite){.tile=&TILES[ (star * 8)+STARS ], .mask=&MASKS[STAR_MASK], .x=c*8, .y=r*8,},
            .active=TRUE,
        };
    }
    
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
        
        for (byte i=0 ; i<MAX_STARS ; i++)
        {
            draw_sprite(&stars[i].sprite);
        }
        
        if (star_timer <= t)
        {
            for (byte i=0 ; i<MAX_STARS ; i++)
            {
                stars[i].sprite.x += stars[i].vx;
                if (stars[i].sprite.x < -8)
                {
                    byte star = rng() & 15;
                    int vx = -1;
                    if (star == 15)
                        vx = -2;
                    
                    byte r = rng() & (SCREEN_ROWS-1);
                        
                    stars[i] = (Mob){
                        .vx=vx,
                        .vy=0,
                        .sprite = (Sprite){.tile=&TILES[ (star * 8)+STARS ], .mask=&MASKS[STAR_MASK], .x=SCREEN_WIDTH+8, .y=r*8,},
                        .active=TRUE,
                    };
                }
            }
            star_timer = t+25;
        }
        
        for (byte i=0 ; i<MAX_MOBS ; i++)
        {
            if (mobs[i].active)
                draw_sprite(&mobs[i].sprite);
            }
            
        draw();
    }
}
