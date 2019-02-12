#ifndef TILE_ENGINE_H
#define TILE_ENGINE_H

#include "SPIKE.h"

typedef struct Sprite {
    int x;
    int y;
    
    const byte __flash *tile;
    const byte __flash *mask;
} Sprite;

typedef struct Map {
    word cols;
    word rows;
    byte tileset;
    byte tiles[];
} Map;

typedef struct Viewport {
    int x;
    int y;
} Viewport;

void draw_tile(const byte __flash *tile, int x, int y);
void draw_tile_masked(const byte __flash *glyph, const byte __flash *mask, int x, int y);
void draw_map(const Map __memx *map, const __flash byte *tileset);
void draw_sprite(Sprite *s);

void center_on_sprite(Sprite *s, const Map __memx *map);

byte collide_with_world(Sprite *s, const Map __memx *map);

#endif