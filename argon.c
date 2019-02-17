#include "argon.h"

Mob mobs[MAX_MOBS];

Mob stars[MAX_STARS];

void argon(void)
{
    dword t = 0;
    dword move_timer = 0;
    dword shoot_timer = 0;
    dword star_timer = 0;
    
    byte buttons;
    
    bool firing = FALSE;
    int hit_x = 0;
    int weapon_temp = 0;

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
        if (shoot_timer <= t)
        {
            if ( buttons & BTN_A )
            {
                if (weapon_temp < (255-WEAPON_HEAT_RATE-1))
                {
                    click();
                    // cast to collision
                    hit_x = SCREEN_WIDTH;
                    firing = TRUE;
                    
                    weapon_temp += WEAPON_HEAT_RATE;
                }
                else {
                    firing = FALSE;
                }
            }
            else 
            {
                firing = FALSE;
                weapon_temp -= WEAPON_COOL_RATE;
                if (weapon_temp < 0)
                    weapon_temp = 0;
            }
            shoot_timer = t+30;
        }
            
        if (move_timer <= t)
        {   
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
            
            move_timer = t+20;
        }
        
        clear_buffer();
        
        for (byte i=0 ; i<MAX_STARS ; i++)
        {
            draw_sprite(&stars[i].sprite);
            
            if (star_timer <= t)
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
                        .sprite = (Sprite){ .tile=&TILES[ (star * 8)+STARS ], 
                                            .mask=&MASKS[STAR_MASK], 
                                            .x=SCREEN_WIDTH+8, 
                                            .y=r*8,
                        },
                        .active=TRUE,
                    };
                }
            }
        }
        
        if (star_timer <= t)
            star_timer = t+25;
        
        for (byte i=0 ; i<MAX_MOBS ; i++)
        {
            if (mobs[i].active)
                draw_sprite(&mobs[i].sprite);
        }
        
        if (firing)
        {
            for (byte x=player->sprite.x+9 ; x<hit_x ; x++)
                draw_pixel(x, player->sprite.y+4);
        }
        
        for(byte i=0 ; i<(weapon_temp >> 2) ; i++)
        {
            buffer[ (7*SCREEN_WIDTH+(4*8)) + i ] |= 0xc0;
        }
            
        draw();
    }
}
