#include "argon.h"

Mob mobs[MAX_MOBS];

Mob stars[MAX_STARS];

void argon(void)
{
    dword t = 0;
    dword move_timer = 0;
    dword shoot_timer = 0;
    dword star_timer = 0;
    dword wave_timer = 0;
    
    byte wave_index = 0;
    
    word score = 0;
    
    byte buttons;
    
    bool firing = FALSE;
    int hit_x = 0;
    int weapon_temp = 0;

    mobs[MAX_MOBS-1] = (Mob){
        .vx=0,
        .vy=0,
        .sprite = (Sprite){.tile=&TILES[PLAYER_SHIP], .mask=&MASKS[PLAYER_SHIP], .x=2, .y=32-4,},
        .active=TRUE,
        
        .type = PLAYER_SHIP,
    };
    
    Mob *player = &mobs[MAX_MOBS-1];
    
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
            
            .type=STARS,
        };
    }
    
    wave_timer = millis() + 3800;
    
    for(ever)
    {
        t = millis();
        
        if (wave_timer <= t)
        {
            for(byte i=0 ; i<8 ; i++)
            {
                if (WAVES[wave_index][i] > 0)
                {
                    mobs[i] = (Mob){
                        .vx=STATS[WAVES[wave_index][i]>>3].vx,
                        .vy=STATS[WAVES[wave_index][i]>>3].vy,
                        .sprite = (Sprite){
                            .tile=&TILES[ WAVES[wave_index][i] ], 
                            .mask=&MASKS[ WAVES[wave_index][i] ], 
                            .x=SCREEN_WIDTH-8, 
                            .y=i*8,
                        },
                        .active=TRUE,
                        
                        .type=WAVES[wave_index][i],
                    };
                }
            }
            
            wave_timer = t+2400;
        }
        
        buttons = read_buttons();
        if (shoot_timer <= t)
        {
            if ( buttons & BTN_A )
            {
                if (weapon_temp < (255-WEAPON_HEAT_RATE-1))
                {
                    click();
                    
                    hit_x = SCREEN_WIDTH;
                    firing = TRUE;
                    
                    for (byte i=0 ; i<MAX_MOBS-1 ; i++)
                    {
                        if (mobs[i].active && 
                            player->sprite.y+4 > mobs[i].sprite.y && 
                            player->sprite.y < mobs[i].sprite.y+8)
                        {
                            hit_x = mobs[i].sprite.x;
                            
                            if ( (mobs[i].type>>3) < 9)
                            {
                                score += STATS[mobs[i].type>>3].score;
                                
                                mobs[i].type = WRECK;
                                mobs[i].vx = 0;
                                mobs[i].vy = 0;
                            }
                        }
                    }
                    
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
            shoot_timer = t+120;
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
            
            for(byte i=0 ; i<MAX_MOBS-1 ; i++)
            {
                if (mobs[i].active)
                {
                    mobs[i].sprite.x += mobs[i].vx;
                    mobs[i].sprite.y += mobs[i].vy;
                    
                    //TODO: State machine
                    if (mobs[i].type == DART && mobs[i].sprite.x < -8)
                        mobs[i].active = FALSE;
                        
                    if (mobs[i].type == WRECK)
                    {
                        mobs[i].type = EXPLOSION_1;
                        mobs[i].sprite.tile = &TILES[ EXPLOSION_1 ];
                        mobs[i].sprite.mask = &MASKS[ STAR_MASK ];
                        mobs[i].animation_timer = t+EXPLOSION_FRAME_LENGTH;
                    }
                    else if (mobs[i].type == EXPLOSION_1 && mobs[i].animation_timer <= t)
                    {
                        mobs[i].type = EXPLOSION_2;
                        mobs[i].sprite.tile = &TILES[ EXPLOSION_2 ];
                        mobs[i].sprite.mask = &MASKS[ STAR_MASK ];
                        mobs[i].animation_timer = t+EXPLOSION_FRAME_LENGTH;
                    }
                    else if (mobs[i].type == EXPLOSION_2 && mobs[i].animation_timer <= t)
                    {
                        mobs[i].type = EXPLOSION_3;
                        mobs[i].sprite.tile = &TILES[ EXPLOSION_3 ];
                        mobs[i].sprite.mask = &MASKS[ STAR_MASK ];
                        mobs[i].animation_timer = t+EXPLOSION_FRAME_LENGTH;
                    }
                    else if (mobs[i].type == EXPLOSION_3 && mobs[i].animation_timer <= t)
                    {
                        mobs[i].type = EXPLOSION_4;
                        mobs[i].sprite.tile = &TILES[ EXPLOSION_4 ];
                        mobs[i].sprite.mask = &MASKS[ STAR_MASK ];
                        mobs[i].animation_timer = t+EXPLOSION_FRAME_LENGTH;
                    }
                    else if (mobs[i].type == EXPLOSION_4 && mobs[i].animation_timer <= t)
                    {
                        mobs[i].active = FALSE;
                    }
                    
                    //Collided with player?
                    
                    if (mobs[i].sprite.x+6 > player->sprite.x+2 &&
                        mobs[i].sprite.x+2 < player->sprite.x+6 &&
                        mobs[i].sprite.y+7 > player->sprite.y+1 &&
                        mobs[i].sprite.y+1 < player->sprite.y+7)
                    {    
                        draw_string(&GAME_OVER[0], 32, 24);
                        
                        draw_tile_masked(&DIGITS[(score % 10)*8], &MASKS[0], 10*8, 40);
                        score /= 10;
                        draw_tile_masked(&DIGITS[(score % 10)*8], &MASKS[0], 9*8, 40);
                        score /= 10;
                        draw_tile_masked(&DIGITS[(score % 10)*8], &MASKS[0], 8*8, 40);
                        score /= 10;
                        draw_tile_masked(&DIGITS[(score % 10)*8], &MASKS[0], 7*8, 40);
                        score /= 10;
                        draw_tile_masked(&DIGITS[(score % 10)*8], &MASKS[0], 6*8, 40);
                        //score /= 10;
                        
                        draw();
                        
                        for(byte j=0 ; j<MAX_MOBS-1 ; j++)
                        {
                            mobs[j].active = FALSE;
                        }
                        dword d = t+8000;
                        while (d>t)
                            t=millis();
                        return;
                    }
                }
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
                        .type=STARS,
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

void draw_string(const __memx char *string, int x, int y)
{
    for(byte i=0 ; string[i] != '\0' ; i++)
    {
        draw_tile_masked(&ASCII[(string[i]-32)*8], &MASKS[0], x+(i*8), y);
    }
}
