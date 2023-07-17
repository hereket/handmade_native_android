

uint32
GetTileValue(tile_map *TileMap, int32 AbsTileX, int32 AbsTileY, int32 AbsTileZ) 
{
    uint32 Result = 0;

    int32 ChunkX = AbsTileX >> TileMap->ChunkShift;
    int32 ChunkY = AbsTileY >> TileMap->ChunkShift;
    int32 ChunkZ = AbsTileZ;

    if((ChunkX >= 0) && (ChunkX < TileMap->TileChunkCountX) &&
       (ChunkY >= 0) && (ChunkY < TileMap->TileChunkCountY) &&
       (ChunkZ >= 0) && (ChunkZ < TileMap->TileChunkCountZ)) 
    {
        tile_chunk *Chunk = TileMap->TileChunks + 
                            ChunkZ * TileMap->TileChunkCountY * TileMap->TileChunkCountX +
                            ChunkY*TileMap->TileChunkCountX + 
                            ChunkX;

        if(Chunk) {
            int32 TileInChunkX = AbsTileX & TileMap->ChunkMask;
            int32 TileInChunkY = AbsTileY & TileMap->ChunkMask;

            if(Chunk->Tiles) {
                uint32 *Tile = Chunk->Tiles + TileInChunkY*TileMap->ChunkDim + TileInChunkX;
                Result = *Tile;
            }
        }
    }

    return Result;
}

uint32 
GetTileValue(tile_map *TileMap, tile_map_position P) 
{
    uint32 Result = GetTileValue(TileMap, P.AbsTileX, P.AbsTileY, P.AbsTileZ);
    return Result;
}

inline bool32
AreOnSameTile(tile_map_position *A, tile_map_position *B)
{
    bool32 Result = ((A->AbsTileX == B->AbsTileX) &&
                     (A->AbsTileY == B->AbsTileY) &&
                     (A->AbsTileZ == B->AbsTileZ));

    return(Result);
}



internal bool32
IsTileValueEmpty(uint32 TileValue)
{
    bool32 Empty = ((TileValue == 1) ||
                    (TileValue == 3) ||
                    (TileValue == 4));

    return(Empty);
}

internal bool32
IsTileMapPointEmpty(tile_map *TileMap, tile_map_position Pos)
{
    uint32 TileChunkValue = GetTileValue(TileMap, Pos);
    bool32 Empty = IsTileValueEmpty(TileChunkValue);

    return(Empty);
}


void
RecanonicalizeCoord(tile_map *TileMap, uint64 *Tile, real32 *TileRel) 
{
    int32 Offset = RoundReal32ToInt32(*TileRel / TileMap->TileSideInMeters);
    *Tile = *Tile + Offset;
    *TileRel = *TileRel - Offset*TileMap->TileSideInMeters;

    // __android_log_print(ANDROID_LOG_DEBUG, "------ TAG", "Offset: %d, TileRel/TileSideInMeters: %f --- %d",
    //         Offset, *TileRel / TileMap->TileSideInMeters, RoundReal32ToInt32(*TileRel / TileMap->TileSideInMeters));

    Assert(*TileRel >= -0.5*TileMap->TileSideInMeters);
    Assert(*TileRel <=  0.5*TileMap->TileSideInMeters);

    // int32 Offset = RoundReal32ToInt32(*TileRel / TileMap->TileSideInMeters);
    // *Tile += Offset;
    // *TileRel -= Offset*TileMap->TileSideInMeters;

    // // TODO(casey): Fix floating point math so this can be < ?
    // Assert(*TileRel >= -0.5f*TileMap->TileSideInMeters);
    // Assert(*TileRel <= 0.5f*TileMap->TileSideInMeters);

}


inline tile_map_position
RecanonicalizePosition(tile_map *TileMap, tile_map_position Pos) 
{
    tile_map_position Result = Pos;

    RecanonicalizeCoord(TileMap, &Result.AbsTileX, &Result.Offset_.X);
    RecanonicalizeCoord(TileMap, &Result.AbsTileY, &Result.Offset_.Y);

    return Result;
}

bool32
AreOnTheSameTile(tile_map_position *A, tile_map_position *B) 
{
    bool32 Result = false;

    if( A->AbsTileX == B->AbsTileX &&
        A->AbsTileY == B->AbsTileY &&
        A->AbsTileZ == B->AbsTileZ)
    {
        Result = true;
    }

    return Result;
}


void
SetTileValue(memory_arena *Arena, tile_map *TileMap, uint32 AbsTileX, uint32 AbsTileY, uint32 AbsTileZ, uint32 Value) 
{
    uint32 ChunkX = AbsTileX >> TileMap->ChunkShift;
    uint32 ChunkY = AbsTileY >> TileMap->ChunkShift;
    uint32 ChunkZ = AbsTileZ;

    tile_chunk *Chunk = TileMap->TileChunks + 
                        ChunkZ * TileMap->TileChunkCountY * TileMap->TileChunkCountX +
                        ChunkY * TileMap->TileChunkCountX +
                        ChunkX;

    if(!Chunk->Tiles) {
        int32 TileCount = TileMap->ChunkDim*TileMap->ChunkDim;
        Chunk->Tiles = (uint32 *)PushArray(Arena, TileCount, uint32*);

        for(int32 TileY=0; TileY<TileMap->ChunkDim; TileY++) {
            for(int32 TileX=0; TileX<TileMap->ChunkDim; TileX++) {
                uint32 *Tile = Chunk->Tiles + TileY*TileMap->ChunkDim + TileX;
                *Tile = 1;
            }
        }
    }

    if(TileMap && Chunk->Tiles) {
        uint32 TileX = AbsTileX & TileMap->ChunkMask;
        uint32 TileY = AbsTileY & TileMap->ChunkMask;
        uint32 TileZ = AbsTileZ & TileMap->ChunkMask;

        uint32 *Tile = Chunk->Tiles + TileY*TileMap->ChunkDim + TileX;
        *Tile = Value;
    }
}

inline tile_map_difference
Subtract(tile_map *TileMap, tile_map_position *A, tile_map_position *B)
{
    tile_map_difference Result = {};

    v2 dTileXY = {
        (real32)A->AbsTileX - (real32)B->AbsTileX,
        (real32)A->AbsTileY - (real32)B->AbsTileY
    };
    real32 dTileZ = (real32)A->AbsTileZ - (real32)B->AbsTileZ;

    Result.dXY = TileMap->TileSideInMeters * dTileXY + (A->Offset_ - B->Offset_);

    return Result;
}

inline tile_map_position
CenteredTilePoint(uint32 AbsTileX, uint32 AbsTileY, uint32 AbsTileZ)
{
    tile_map_position Result = {};

    Result.AbsTileX = AbsTileX;
    Result.AbsTileY = AbsTileY;
    Result.AbsTileZ = AbsTileZ;

    return(Result);
}


inline tile_map_position
Offset(tile_map *TileMap, tile_map_position P, v2 Offset)
{
    P.Offset_ += Offset;
    P = RecanonicalizePosition(TileMap, P);

    return(P);
}


