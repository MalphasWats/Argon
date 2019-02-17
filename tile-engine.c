#include "tile-engine.h"

Viewport viewport = {.x=0, .y=0};

void draw_tile_masked(const byte __flash *tile, const byte __flash *mask, int x, int y)
{
    /* is the tile actually visible */
    if (x < -7 || x >= SCREEN_WIDTH || y < -7 || y >= SCREEN_HEIGHT)
        return;
    
    
    int y_ = y;
    
    if (y < 0)
        y_ = 0-y;
        
    int tile_start = ((y_ >> 3) * SCREEN_WIDTH) + x;
    
    byte y_offset_a = y & 7; // y % 8
    byte y_offset_b = 8-y_offset_a;
    
    byte tile_index = 0;
    byte tile_width = 8;
    if (x < 0)
    {
        tile_start -= x;
        tile_index = 0-x;
        tile_width -= tile_index;
    }
    
    if (x > SCREEN_WIDTH-8)
    {
        tile_width = SCREEN_WIDTH-x;
    }
    
    if (y < 0)
    {
        y_offset_a = 8;
        y_offset_b = 0-y;
        tile_start -= SCREEN_WIDTH;
    }
    
    if (y > SCREEN_HEIGHT-8)
    {
        y_offset_b = 8;
    }
    
    for(byte tile_offset=0 ; tile_offset<tile_width ; tile_offset++, tile_index++)
    {
        if (y_offset_a < 8)
        {
            buffer[tile_start+tile_offset] &= (mask[tile_index] << y_offset_a) | ~(0xff << y_offset_a);
            buffer[tile_start+tile_offset] |= tile[tile_index] << y_offset_a;
        }
        if (y_offset_b < 8)
        {
            buffer[tile_start+SCREEN_WIDTH+tile_offset] &= (mask[tile_index] >> y_offset_b) | ~(0xff >> y_offset_b);
            buffer[tile_start+SCREEN_WIDTH+tile_offset] |= tile[tile_index] >> y_offset_b;
        }
    }
}

void draw_tile(const byte __flash *tile, int x, int y)
{
    /* is the tile actually visible */
    if (x < -7 || x >= SCREEN_WIDTH || y < -7 || y >= SCREEN_HEIGHT)
        return;
    
    
    int y_ = y;
    
    if (y < 0)
        y_ = 0-y;
        
    int tile_start = ((y_ >> 3) * SCREEN_WIDTH) + x;
    
    byte y_offset_a = y & 7; // y % 8
    byte y_offset_b = 8-y_offset_a;
    
    byte tile_index = 0;
    byte tile_width = 8;
    if (x < 0)
    {
        tile_start -= x;
        tile_index = 0-x;
        tile_width -= tile_index;
    }
    
    if (x > SCREEN_WIDTH-8)
    {
        tile_width = SCREEN_WIDTH-x;
    }
    
    if (y < 0)
    {
        y_offset_a = 8;
        y_offset_b = 0-y;
        tile_start -= SCREEN_WIDTH;
    }
    
    if (y > SCREEN_HEIGHT-8)
    {
        y_offset_b = 8;
    }
    
    for(byte tile_offset=0 ; tile_offset<tile_width ; tile_offset++, tile_index++)
    {
        if (y_offset_a < 8)
        {
            buffer[tile_start+tile_offset] |= tile[tile_index] << y_offset_a;
        }
        if (y_offset_b < 8)
        {
            buffer[tile_start+SCREEN_WIDTH+tile_offset] |= tile[tile_index] >> y_offset_b;
        }
    }
}

void draw_map(const Map __memx *map, const __flash byte *tileset)
{   
    int x = viewport.x;
    int y = viewport.y;
    byte x_offset = x & 7; // x % 8
    x >>= 3;
    
    byte y_offset = y & 7; // y % 8
    y >>= 3;
    
    byte NUM_ROWS = SCREEN_ROWS;
    if (y_offset > 0)
        NUM_ROWS += 1;
    
    byte NUM_COLS = SCREEN_COLUMNS;
    if (x_offset > 0)
        NUM_COLS += 1;
    
    for (byte row=0 ; row<NUM_ROWS ; row++)
    {
        for (byte col=0 ; col<NUM_COLS ; col++)
        {
            draw_tile(&tileset[map->tiles[map->cols * (row+y) + (col+x)]*8], col*8-x_offset, row*8-y_offset); 
        }
    }
}

void draw_sprite(Sprite *s)
{
    draw_tile_masked(s->tile, s->mask, s->x-viewport.x, s->y-viewport.y);
}

void center_on_sprite(Sprite *s, const Map __memx *map)
{
    viewport.x = s->x - (SCREEN_WIDTH>>1);
    viewport.y = s->y - (SCREEN_HEIGHT>>1);
    
    if (viewport.x < 0)
        viewport.x = 0;
    if (viewport.y < 0)
        viewport.y = 0;
        
    if (viewport.x > map->cols*8 - SCREEN_WIDTH)
        viewport.x = map->cols*8 - SCREEN_WIDTH;
    if (viewport.y > map->rows*8 - SCREEN_HEIGHT)
        viewport.y = map->rows*8 - SCREEN_HEIGHT;    
    
}