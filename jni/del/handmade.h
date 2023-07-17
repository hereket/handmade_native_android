#ifndef __HANDMADE_H__
#define __HANDMADE_H__

// #include "handmade_tile.h"
#include "handmade_platform.h"
#include "handmade_math.h"

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))


typedef struct {
    char *Memory;
    int Width;
    int Height;
    int Stride;
} buffer;


typedef struct {
    v2 P;
    v2 Size;
} rect;



typedef uint64 memory_index;

typedef struct {
    uint8 *Base;
    memory_index Size;
    memory_index Used;
} memory_arena;

internal void
InitializeArena(memory_arena *Arena, uint8 *Base, uint64 Size) 
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}





#include <android/log.h>

#include "handmade_math.h"

#include "handmade_tile.h"



#define PushStruct(Arena, type) PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) PushSize_(Arena, sizeof(type) * (Count))
void *
PushSize_(memory_arena *Arena, memory_index Size) 
{
    Assert(Arena->Used + Size <= Arena->Size);

    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;

    return Result;
}



typedef struct {
    int32 Width;
    int32 Height;
    uint8 *Pixels;
} loaded_bitmap;


typedef struct {
    int32 AlignX;
    int32 AlignY;
    loaded_bitmap Head;
    loaded_bitmap Cape;
    loaded_bitmap Torso;
} hero_bitmaps;




typedef struct {
    tile_map *TileMap;
} world;

typedef struct
{
    bool32 Exists;
    tile_map_position P;
    v2 dP;
    uint32 FacingDirection;
    real32 Width, Height;
} entity;


typedef struct {
    // tile_map_position PlayerP;
    memory_arena WorldArena;
    world *World;

    uint32 CameraFollowingEntityIndex;
    tile_map_position CameraP;

    tile_map_position PlayerP;

    uint32 PlayerIndexForController[ArrayCount(((game_input *)0)->Controllers)];
    uint32 EntityCount;
    entity Entities[256];

    loaded_bitmap Backdrop;
    hero_bitmaps HeroBitmaps[4];
} game_state;


#define GAME_UPDATE_AND_RENDER(name) void name(buffer *Buffer, game_input *Input, game_memory *Memory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);


#endif
