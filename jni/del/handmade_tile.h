#ifndef _HANDMADE_TILE_H_
#define  _HANDMADE_TILE_H_

typedef struct {
    v2 dXY;
    real32 dZ;
} tile_map_difference;

typedef struct {
    uint64 AbsTileX;
    uint64 AbsTileY;
    uint64 AbsTileZ;

    v2 Offset_;
} tile_map_position;


typedef struct {
    uint32 *Tiles;
} tile_chunk;

typedef struct {
    uint32 ChunkDim;
    uint32 ChunkShift;
    uint32 ChunkMask;

    uint32 ScreenTileCountX;
    uint32 ScreenTileCountY;

    uint32 TileChunkCountX;
    uint32 TileChunkCountY;
    uint32 TileChunkCountZ;

    real32 TileSideInMeters;

    tile_chunk *TileChunks;
} tile_map;



#endif
