#include "argon.h"

Mob mobs[MAX_MOBS];

Mob stars[MAX_STARS];

void argon(void)
{
    uint32_t t = 0;
    uint32_t move_timer = 0;
    uint32_t shoot_timer = 0;
    uint32_t star_timer = 0;
    uint32_t wave_timer = 0;
    
    uint8_t wave_index = 0;
    
    uint16_t score = 0;
    
    uint8_t buttons;
    
    bool firing = FALSE;
    int16_t hit_x = 0;
    int16_t weapon_temp = 0;

    mobs[MAX_MOBS-1] = (Mob){
        .vx=0,
        .vy=0,
        .sprite = (Sprite){.tile=&TILES[PLAYER_SHIP], .mask=&MASKS[PLAYER_SHIP], .x=2, .y=32-4,},
        .active=TRUE,
        
        .type = PLAYER_SHIP,
    };
    
    Mob *player = &mobs[MAX_MOBS-1];
    
    for(uint8_t i=0 ; i<MAX_STARS ; i++)
    {
        uint8_t star = rng() & 15;
            
        uint8_t c = rng() & (SCREEN_COLUMNS-1);
        uint8_t r = rng() & (SCREEN_ROWS-1);
            
        stars[i] = (Mob){
            .vx=-1,
            .vy=0,
            .sprite = (Sprite){.tile=&TILES[ (star * 8)+STARS ], .mask=&MASKS[STAR_MASK], .x=c*8, .y=r*8,},
            .active=TRUE,
            
            .type=STARS,
        };
    }
    
    wave_timer = millis() + 2800;
    
    for(ever)
    {
        t = millis();
        
        if (wave_timer <= t)
        {
            for(uint8_t i=0 ; i<8 ; i++)
            {
                if (WAVES[wave_index][i] > 0)
                {
                    for(uint8_t m=0; m<MAX_MOBS-1 ; m++)
                    {
                        if (!mobs[m].active)
                        {
                            mobs[m] = (Mob){
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
                            break;
                        }
                    }
                }
            }
            
            wave_timer = t+WAVES[wave_index][8];
            wave_index += 1;
            if (wave_index == WAVE_COUNT)
                wave_index = 0;
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
                    
                    for (uint8_t i=0 ; i<MAX_MOBS-1 ; i++)
                    {
                        if (mobs[i].active && 
                            player->sprite.y+4 > mobs[i].sprite.y && 
                            player->sprite.y < mobs[i].sprite.y+8 &&
                            (   mobs[i].type == FRIGATE ||
                                mobs[i].type == DART ||
                                mobs[i].type == SAUCER ||
                                mobs[i].type == FIGHTER) )
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
            
            for(uint8_t i=0 ; i<MAX_MOBS-1 ; i++)
            {
                if (mobs[i].active)
                {
                    mobs[i].sprite.x += mobs[i].vx;
                    mobs[i].sprite.y += mobs[i].vy;
                    
                    //TODO: State machine
                    if (mobs[i].type == DART && mobs[i].sprite.x < -8)
                        mobs[i].active = FALSE;
                        
                    if (mobs[i].type == FRIGATE && (mobs[i].sprite.y < 0 || mobs[i].sprite.y>SCREEN_HEIGHT-8))
                        mobs[i].vy *= -1;
                        
                    if (mobs[i].type == FRIGATE && (mobs[i].sprite.y == player->sprite.y))
                    {
                        for (uint8_t m=0 ; m<MAX_MOBS-1 ; m++)
                        {
                            if (!mobs[m].active)
                            {
                                mobs[m] = (Mob){
                                    .vx=STATS[MISSILE>>3].vx,
                                    .vy=STATS[MISSILE>>3].vy,
                                    .sprite = (Sprite){
                                        .tile=&TILES[ MISSILE ], 
                                        .mask=&MASKS[ MISSILE ], 
                                        .x=SCREEN_WIDTH-12, 
                                        .y=mobs[i].sprite.y+4,
                                    },
                                    .active=TRUE,
                                    
                                    .type=MISSILE,
                                };
                    
                                break;
                            }
                        }
                    }
                    
                    if (mobs[i].type == FIGHTER && player->sprite.y > mobs[i].sprite.y)
                        mobs[i].vy = 1;
                    else if (mobs[i].type == FIGHTER && player->sprite.y < mobs[i].sprite.y)
                        mobs[i].vy = -1;
                    else if (mobs[i].type == FIGHTER && player->sprite.y == mobs[i].sprite.y && mobs[i].animation_timer <= t)
                    {
                        mobs[i].vy = 0;
                        
                        for (uint8_t m=0 ; m<MAX_MOBS-1 ; m++)
                        {
                            if (!mobs[m].active)
                            {
                                mobs[m] = (Mob){
                                    .vx=STATS[PLASMA_BOLT>>3].vx,
                                    .vy=STATS[PLASMA_BOLT>>3].vy,
                                    .sprite = (Sprite){
                                        .tile=&TILES[ PLASMA_BOLT ], 
                                        .mask=&MASKS[ PLASMA_BOLT ], 
                                        .x=mobs[i].sprite.x, 
                                        .y=mobs[i].sprite.y+4,
                                    },
                                    .active=TRUE,
                                    
                                    .type=PLASMA_BOLT,
                                };
                                mobs[i].animation_timer = t+160;
                                break;
                            }
                        }
                    }
                    
                    if (mobs[i].type == FIGHTER && (mobs[i].sprite.x < 64 || mobs[i].sprite.x > SCREEN_WIDTH-8))
                        mobs[i].vx *= -1;
                    
                    if(mobs[i].type == FIGHTER)
                    {
                        for(uint8_t j=0 ; j<MAX_MOBS-1 ; j++)
                        {
                            if (j != i &&
                                mobs[j].type == FIGHTER &&
                                mobs[i].sprite.x+8 > mobs[j].sprite.x &&
                                mobs[i].sprite.x < mobs[j].sprite.x+8 &&
                                mobs[i].sprite.y+8 > mobs[j].sprite.y &&
                                mobs[i].sprite.y < mobs[j].sprite.y+8)
                            {
                                mobs[i].sprite.y += (-1*mobs[i].vy)*2;
                                mobs[j].sprite.y += mobs[i].vy*2;
                                
                                mobs[i].vx *= -1;
                            }
                        }
                    }
                    
                    if(mobs[i].type == SAUCER)
                    {
                        if (mobs[i].animation_timer <= t)
                        {
                            if (mobs[i].vy != 0)
                            {
                                mobs[i].vy = 0;
                                mobs[i].animation_timer = t+700;
                            }
                            else 
                            {
                                mobs[i].animation_timer = t+700;
                                for (uint8_t m=0 ; m<MAX_MOBS-1 ; m++)
                                {
                                    if (!mobs[m].active)
                                    {
                                        mobs[m] = (Mob){
                                            .vx=STATS[MINE>>3].vx,
                                            .vy=STATS[MINE>>3].vy,
                                            .sprite = (Sprite){
                                                .tile=&TILES[ MINE ], 
                                                .mask=&MASKS[ MINE ], 
                                                .x=mobs[i].sprite.x+10, 
                                                .y=mobs[i].sprite.y+4,
                                            },
                                            .active=TRUE,
                                            
                                            .type=MINE,
                                        };
                            
                                        break;
                                    }
                                }
                                if (rng() & 1)
                                {
                                    mobs[i].vy = -1;
                                }
                                else 
                                {
                                    mobs[i].vy = 1;
                                }
                            }
                        }
                        
                        if (mobs[i].sprite.y < 0 || mobs[i].sprite.y > SCREEN_HEIGHT-8)
                            mobs[i].vy *= -1;
                    }
                    
                    // CLEANUP    
                    if ( (mobs[i].type == PLASMA_BOLT || mobs[i].type == MISSILE || mobs[i].type == MINE) && mobs[i].sprite.x < 0)
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
                        
                        draw_int(score, 5, 6*8, 5*8);
                        
                        draw();
                        
                        for(uint8_t j=0 ; j<MAX_MOBS-1 ; j++)
                        {
                            mobs[j].active = FALSE;
                        }
                        uint32_t d = t+8000;
                        while (d>t)
                            t=millis();
                        return;
                    }
                }
            }
            
            move_timer = t+20;
        }
        
        clear_buffer();
        
        for (uint8_t i=0 ; i<MAX_STARS ; i++)
        {
            draw_sprite(&stars[i].sprite);
            
            if (star_timer <= t)
            {
                stars[i].sprite.x += stars[i].vx;
                if (stars[i].sprite.x < -8)
                {
                    uint8_t star = rng() & 15;
                    
                    uint8_t r = rng() & (SCREEN_ROWS-1);
                        
                    stars[i] = (Mob){
                        .vx=-1,
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
        
        for (uint8_t i=0 ; i<MAX_MOBS ; i++)
        {
            if (mobs[i].active)
                draw_sprite(&mobs[i].sprite);
        }
        
        if (firing)
        {
            for (uint8_t x=player->sprite.x+9 ; x<hit_x ; x++)
                draw_pixel(x, player->sprite.y+4);
        }
        
        for(uint8_t i=0 ; i<(weapon_temp >> 2) ; i++)
        {
            buffer[ (7*SCREEN_WIDTH+(4*8)) + i ] |= 0xc0;
        }
        
        draw();
    }
}