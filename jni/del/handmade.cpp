#include "handmade.h"
#include "handmade_random.h"
#include "handmade_intrinsics.h"
#include <android/log.h>

#include "handmade_tile.cpp"

// #include <math.h>

/* __android_log_print(ANDROID_LOG_DEBUG, "----- TAG", "MinX: %d, MinY: %d, MaxX: %d, MaxY: %d", MinX, MinY, MaxX, MaxY); */


internal void
GameOutputSound(game_state *GameState, game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    int16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    int16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        // TODO(casey): Draw this out for people
#if 0
        real32 SineValue = sinf(GameState->tSine);
        int16 SampleValue = (int16)(SineValue * ToneVolume);
#else
        int16 SampleValue = 0;
#endif
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;
#if 0
        GameState->tSine += 2.0f*Pi32*1.0f/(real32)WavePeriod;
        if(GameState->tSine > 2.0f*Pi32)
        {
            GameState->tSine -= 2.0f*Pi32;
        }
#endif
    }
}


internal void 
DrawRectangle(buffer *Buffer, v2 vMin, v2 vMax, real32 R, real32 G, real32 B) 
{
    int32 MinX = RoundReal32ToInt32(vMin.X);
    int32 MinY = RoundReal32ToInt32(vMin.Y);
    int32 MaxX = RoundReal32ToInt32(vMax.X);
    int32 MaxY = RoundReal32ToInt32(vMax.Y);

    if(MinX < 0)              { MinX = 0; }
    if(MinY < 0)              { MinY = 0; }
    if(MaxX > Buffer->Width)  { MaxX = Buffer->Width; }
    if(MaxY > Buffer->Height) { MaxY = Buffer->Height; }

    int32 BytesPerPixel = 4;


    // uint32 Color = (
    //             0x00 << 24 |
    //             (RoundReal32ToInt32(B * 255.0f) << 16) |
    //             (RoundReal32ToInt32(G * 255.0f) << 8) |
    //             (RoundReal32ToInt32(R * 255.0f) << 0)
    //         );

    uint32 Color = (
                0xff << 24 |
                (RoundReal32ToInt32(B * 255.0f) << 16) |
                (RoundReal32ToInt32(G * 255.0f) << 8) |
                (RoundReal32ToInt32(R * 255.0f) << 0)
            );

    uint8 *Row = (uint8 *)Buffer->Memory + (MinY*Buffer->Stride) + MinX*BytesPerPixel;

    for(int32 Y = MinY; Y < MaxY; Y++) {
        int32 *Pixel = (int32 *)Row;
        for(int32 X = MinX; X < MaxX; X++) {
            *Pixel++ = Color;
        }
        Row += Buffer->Stride;
    }
}

internal void
DrawButton(buffer *Buffer, button Button) 
{
    v2 MinP = {Button.PositionX, Button.PositionY};
    v2 MaxP = {Button.PositionX + Button.Width, Button.PositionY + Button.Height};


    // NOTE: TestChange
    real32 R = 1.0f;
    real32 G = 0.0f;
    real32 B = 0.0f;
    if(Button.IsPressed) {
        R = 1.0f;
        G = 0.5f;
        B = 0.5f;
    }
    
    DrawRectangle(Buffer, MinP, MaxP, R, G, B);
}

void
DrawBitmap(buffer *Buffer, loaded_bitmap *Bitmap, 
        real32 StartX, real32 StartY,
        int32 AlignX = 0, int32 AlignY = 0) 
{
    StartX -= (real32)AlignX;
    StartY -= (real32)AlignY;

    int32 MinX = RoundReal32ToInt32(StartX);
    int32 MinY = RoundReal32ToInt32(StartY);
    int32 MaxX = RoundReal32ToInt32(StartX + (real32)Bitmap->Width);
    int32 MaxY = RoundReal32ToInt32(StartY + (real32)Bitmap->Height);

    int32 BytesPerPixel = 4;

    int32 SourceOffsetX = 0;
    int32 SourceOffsetY = 0;

    if(MinX < 0) { 
        SourceOffsetX = -MinX;
        MinX = 0; 
    }

    if(MinY < 0) { 
        SourceOffsetY = -MinY;
        MinY = 0; 
    }

    if(MaxX > Buffer->Width)  { MaxX = Buffer->Width; }
    if(MaxY > Buffer->Height) { MaxY = Buffer->Height; }

    uint32 *SourceRow = ((uint32 *)Bitmap->Pixels) + Bitmap->Width*(Bitmap->Height - 1);
    SourceRow += -SourceOffsetY*Bitmap->Width + SourceOffsetX;
    uint8 *DestRow = ((uint8 *)Buffer->Memory) + MinY*Buffer->Stride + MinX*BytesPerPixel;


    for(int32 Y = MinY; Y < MaxY ; Y++) {
        uint32 *Dest = (uint32 *)DestRow;
        uint32 *Source = SourceRow;

        for(int32 X = MinX; X < MaxX; X++) {

            real32 A = (real32)((*Source >> 24) & 0xFF) / 255;
            // A = 0.1f;

            real32 SourceB = (real32)((*Source >> 16) & 0xFF);
            real32 SourceG = (real32)((*Source >> 8) & 0xFF);
            real32 SourceR = (real32)((*Source >> 0) & 0xFF);

            real32 DestB = (real32)((*Dest >> 16) & 0xFF);
            real32 DestG = (real32)((*Dest >> 8) & 0xFF);
            real32 DestR = (real32)((*Dest >> 0) & 0xFF);

            real32 FinalR = (1.0f - A)*DestR + A*SourceR;
            real32 FinalG = (1.0f - A)*DestG + A*SourceG;
            real32 FinalB = (1.0f - A)*DestB + A*SourceB;


            *Dest = (0xFF << 24 |
                ((uint32)(FinalB + 0.5f) << 16) |
                ((uint32)(FinalG + 0.5f) << 8) |
                ((uint32)(FinalR + 0.5f) << 0));

            // if(*Dest == 0) {
                // *Dest = 0xffffffff;
            // }

            Dest++;
            Source++;
        }

        DestRow += Buffer->Stride;
        SourceRow -= Bitmap->Width;
    }
}

#pragma pack(1)
typedef struct {
    uint8 Signature[2];
    int32 FileSize;
    int32 Reserved;
    int32 DataOffset;

    int32 Size;
    int32 Width;
    int32 Height;
    int16 PlaneCount;
    int16 BitCount;
    int32 Compression;
    int32 BitmapSize;
    int32 HorizontalResolution;
    int32 VerticalResolution;
    int32 ColorsUsed;
    int32 ColorsImportant;

    uint32 RedMask;
    uint32 GreenMask;
    uint32 BlueMask;
    uint32 AlphaMask;
    
} bmp_header;
#pragma pack()

int32
GetFirstSetBitOffset(uint32 Value) {
    int32 Result = 0;

    for(int32 Index = 0; Index < sizeof(Value)*8; Index++) {
        if(((Value >> Index) & 1) == 1) { break; }
        Result++;
    }

    return Result;
}

loaded_bitmap
DEBUGLoadBMP(game_memory *Memory, uint8 *Path) {
    loaded_bitmap Result = {};

    file_read_result ReadResult = Memory->DEBUGReadEntireFile(Path);

    if(ReadResult.FileSize > 0) {
        uint8 *Pointer = ReadResult.Contents;

        bmp_header *Header =  (bmp_header *)Pointer;

        Result.Pixels = ReadResult.Contents + Header->DataOffset;
        Result.Width = Header->Width;
        Result.Height = Header->Height;

        uint32 RedShift = GetFirstSetBitOffset(Header->RedMask);
        uint32 GreenShift = GetFirstSetBitOffset(Header->GreenMask);
        uint32 BlueShift = GetFirstSetBitOffset(Header->BlueMask);
        uint32 AlphaShift = GetFirstSetBitOffset(Header->AlphaMask);

        uint32 *Pixel = (uint32 *)Result.Pixels;
        for(int32 Y = 0; Y < Result.Height; Y++) {
            for(int32 X = 0; X < Result.Width; X++) {

                uint32 C = *Pixel;

                uint32 Red   = (C >> RedShift) & 0xFF;
                uint32 Green = (C >> GreenShift) & 0xFF;
                uint32 Blue  = (C >> BlueShift) & 0xFF;
                uint32 Alpha  = (C >> AlphaShift) & 0xFF;

                *Pixel++ = Alpha << 24 | Blue << 16 | Green << 8 | Red;
            }
        }
    }


    return Result;
}

inline real32
Lerp(real32 A, real32 B, real32 t) {
    real32 Result = 0;

    Result = A * (1 - t) + B * t;

    return Result;
}


inline entity *
GetEntity(game_state *GameState, uint32 Index)
{
    entity *Entity = 0;
    
    if((Index > 0) && (Index < ArrayCount(GameState->Entities)))
    {
        Entity = &GameState->Entities[Index];
    }

    return(Entity);
}

internal void
InitializePlayer(game_state *GameState, uint32 EntityIndex)
{
    entity *Entity = GetEntity(GameState, EntityIndex);
    
    Entity->Exists = true;
    Entity->P.AbsTileX = 1;
    Entity->P.AbsTileY = 3;
    Entity->P.Offset_.X = 0;
    Entity->P.Offset_.Y = 0;
    Entity->Height = 0.5f; // 1.4f;
    Entity->Width = 1.0f;

    if(!GetEntity(GameState, GameState->CameraFollowingEntityIndex))
    {
        GameState->CameraFollowingEntityIndex = EntityIndex;
    }
}

internal uint32
AddEntity(game_state *GameState)
{
    uint32 EntityIndex = GameState->EntityCount++;

    Assert(GameState->EntityCount < ArrayCount(GameState->Entities));
    entity *Entity = &GameState->Entities[EntityIndex];
    *Entity = {};
    
    return(EntityIndex);
}

internal bool32
TestWall(real32 WallX, real32 RelX, real32 RelY, real32 PlayerDeltaX, real32 PlayerDeltaY,
         real32 *tMin, real32 MinY, real32 MaxY)
{
    bool32 Hit = false;
    
    real32 tEpsilon = 0.00001f;
    if(PlayerDeltaX != 0.0f)
    {
        real32 tResult = (WallX - RelX) / PlayerDeltaX;
        real32 Y = RelY + tResult*PlayerDeltaY;
        if((tResult >= 0.0f) && (*tMin > tResult))
        {
            if((Y >= MinY) && (Y <= MaxY))
            {
                *tMin = Maximum(0.0f, tResult - tEpsilon);
                Hit = true;
            }
        }
    }

    return(Hit);
}


internal void
MovePlayer(game_state *GameState, entity *Entity, real32 dt, v2 ddP)
{
    tile_map *TileMap = GameState->World->TileMap;

    real32 ddPLength = LengthSq(ddP);
    if(ddPLength > 1.0f)
    {
        ddP *= (1.0f / SquareRoot(ddPLength));
    }

    real32 PlayerSpeed = 50.0f; // m/s^2
    ddP *= PlayerSpeed;

    // TODO(casey): ODE here!
    ddP += -8.0f*Entity->dP;

    tile_map_position OldPlayerP = Entity->P;
    v2 PlayerDelta = (0.5f*ddP*Square(dt) +
                      Entity->dP*dt);
    Entity->dP = ddP*dt + Entity->dP;
    tile_map_position NewPlayerP = Offset(TileMap, OldPlayerP, PlayerDelta);

#if 0
    // TODO(casey): Delta function that auto-recanonicalizes

    tile_map_position PlayerLeft = NewPlayerP;
    PlayerLeft.Offset_.X -= 0.5f*Entity->Width;
    PlayerLeft = RecanonicalizePosition(TileMap, PlayerLeft);
            
    tile_map_position PlayerRight = NewPlayerP;
    PlayerRight.Offset_.X += 0.5f*Entity->Width;
    PlayerRight = RecanonicalizePosition(TileMap, PlayerRight);

    bool32 Collided = false;
    tile_map_position ColP = {};
    if(!IsTileMapPointEmpty(TileMap, NewPlayerP))
    {
        ColP = NewPlayerP;
        Collided = true;
    }
    if(!IsTileMapPointEmpty(TileMap, PlayerLeft))
    {
        ColP = PlayerLeft;
        Collided = true;
    }
    if(!IsTileMapPointEmpty(TileMap, PlayerRight))
    {
        ColP = PlayerRight;
        Collided = true;
    }
            
    if(Collided)
    {
        v2 r = {0,0};
        if(ColP.AbsTileX < Entity->P.AbsTileX)
        {
            r = v2{1, 0};
        }
        if(ColP.AbsTileX > Entity->P.AbsTileX)
        {
            r = v2{-1, 0};
        }
        if(ColP.AbsTileY < Entity->P.AbsTileY)
        {
            r = v2{0, 1};
        }
        if(ColP.AbsTileY > Entity->P.AbsTileY)
        {
            r = v2{0, -1};
        }

        Entity->dP = Entity->dP - 1*Inner(Entity->dP, r)*r;
    }
    else
    {
        Entity->P = NewPlayerP;
    }
#else
    
    uint32 MinTileX = Minimum(OldPlayerP.AbsTileX, NewPlayerP.AbsTileX);
    uint32 MinTileY = Minimum(OldPlayerP.AbsTileY, NewPlayerP.AbsTileY);
    uint32 MaxTileX = Maximum(OldPlayerP.AbsTileX, NewPlayerP.AbsTileX);
    uint32 MaxTileY = Maximum(OldPlayerP.AbsTileY, NewPlayerP.AbsTileY);

    uint32 EntityTileWidth = CeilReal32ToInt32(Entity->Width / TileMap->TileSideInMeters);
    uint32 EntityTileHeight = CeilReal32ToInt32(Entity->Height / TileMap->TileSideInMeters);
    
    MinTileX -= EntityTileWidth;
    MinTileY -= EntityTileHeight;
    MaxTileX += EntityTileWidth;
    MaxTileY += EntityTileHeight;
    
    uint32 AbsTileZ = Entity->P.AbsTileZ;

    real32 tRemaining = 1.0f;
    for(uint32 Iteration = 0;
        (Iteration < 4) && (tRemaining > 0.0f);
        ++Iteration)
    {
        real32 tMin = 1.0f;
        v2 WallNormal = {};

        Assert((MaxTileX - MinTileX) < 32);
        Assert((MaxTileY - MinTileY) < 32);
    
        for(uint32 AbsTileY = MinTileY;
            AbsTileY <= MaxTileY;
            ++AbsTileY)
        {
            for(uint32 AbsTileX = MinTileX;
                AbsTileX <= MaxTileX;
                ++AbsTileX)
            {
                tile_map_position TestTileP = CenteredTilePoint(AbsTileX, AbsTileY, AbsTileZ);
                uint32 TileValue = GetTileValue(TileMap, TestTileP);
                if(!IsTileValueEmpty(TileValue))
                {
                    real32 DiameterW = TileMap->TileSideInMeters + Entity->Width;
                    real32 DiameterH = TileMap->TileSideInMeters + Entity->Height;
                    v2 MinCorner = -0.5f*v2{DiameterW, DiameterH};
                    v2 MaxCorner = 0.5f*v2{DiameterW, DiameterH};

                    tile_map_difference RelOldPlayerP = Subtract(TileMap, &Entity->P, &TestTileP);
                    v2 Rel = RelOldPlayerP.dXY;

                    if(TestWall(MinCorner.X, Rel.X, Rel.Y, PlayerDelta.X, PlayerDelta.Y,
                                &tMin, MinCorner.Y, MaxCorner.Y))
                    {
                        WallNormal = v2{-1, 0};
                    }
                
                    if(TestWall(MaxCorner.X, Rel.X, Rel.Y, PlayerDelta.X, PlayerDelta.Y,
                                &tMin, MinCorner.Y, MaxCorner.Y))
                    {
                        WallNormal = v2{1, 0};
                    }
                
                    if(TestWall(MinCorner.Y, Rel.Y, Rel.X, PlayerDelta.Y, PlayerDelta.X,
                                &tMin, MinCorner.X, MaxCorner.X))
                    {
                        WallNormal = v2{0, -1};
                    }
                
                    if(TestWall(MaxCorner.Y, Rel.Y, Rel.X, PlayerDelta.Y, PlayerDelta.X,
                                &tMin, MinCorner.X, MaxCorner.X))
                    {
                        WallNormal = v2{0, 1};
                    }
                }
            }
        }

        Entity->P = Offset(TileMap, Entity->P, tMin*PlayerDelta);
        Entity->dP = Entity->dP - 1*Inner(Entity->dP, WallNormal)*WallNormal;
        PlayerDelta = PlayerDelta - 1*Inner(PlayerDelta, WallNormal)*WallNormal;
        tRemaining -= tMin*tRemaining;
    }    
#endif

    //
    // NOTE(casey): Update camera/player Z based on last movement.
    //
    if(!AreOnSameTile(&OldPlayerP, &Entity->P))
    {
        uint32 NewTileValue = GetTileValue(TileMap, Entity->P);

        if(NewTileValue == 3)
        {
            ++Entity->P.AbsTileZ;
        }
        else if(NewTileValue == 4)
        {
            --Entity->P.AbsTileZ;
        }    
    }

    if((Entity->dP.X == 0.0f) && (Entity->dP.Y == 0.0f))
    {
        // NOTE(casey): Leave FacingDirection whatever it was
    }
    else if(AbsoluteValue(Entity->dP.X) > AbsoluteValue(Entity->dP.Y))
    {
        if(Entity->dP.X > 0)
        {
            Entity->FacingDirection = 0;
        }
        else
        {
            Entity->FacingDirection = 2;
        }
    }
    else
    {
        if(Entity->dP.Y > 0)
        {
            Entity->FacingDirection = 1;
        }
        else
        {
            Entity->FacingDirection = 3;
        }
    }
}




extern "C"
GAME_UPDATE_AND_RENDER(GameUpdateAndRender) 
{
    real32 PlayerHeight = 1.4;
    real32 PlayerWidth = 0.75*PlayerHeight;

    v2 MinP = {0, 0};
    v2 MaxP = {(real32)Buffer->Width, (real32)Buffer->Height};

    game_state *GameState = (game_state *)Memory->PermanentStorage;

    if(!Memory->IsInitialized) {
        AddEntity(GameState);

        GameState->Backdrop = DEBUGLoadBMP(Memory, (uint8 *)"test/test_background.bmp");

        hero_bitmaps *Bitmap;
        Bitmap = GameState->HeroBitmaps;

        Bitmap->Head  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_right_head.bmp");
        Bitmap->Torso = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_right_torso.bmp");
        Bitmap->Cape  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_right_cape.bmp");
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 185;
        ++Bitmap;

        Bitmap->Head  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_back_head.bmp");
        Bitmap->Torso = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_back_torso.bmp");
        Bitmap->Cape  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_back_cape.bmp");
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 185;
        ++Bitmap;


        Bitmap->Head  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_left_head.bmp");
        Bitmap->Torso = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_left_torso.bmp");
        Bitmap->Cape  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_left_cape.bmp");
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 185;
        ++Bitmap;

        Bitmap->Head  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_front_head.bmp");
        Bitmap->Torso = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_front_torso.bmp");
        Bitmap->Cape  = DEBUGLoadBMP(Memory, (uint8 *)"test/test_hero_front_cape.bmp");
        Bitmap->AlignX = 72;
        Bitmap->AlignY = 185;
        ++Bitmap;

        GameState->CameraP.AbsTileX = 17/2;
        GameState->CameraP.AbsTileY = 9/2;

        InitializeArena(&GameState->WorldArena, 
                        (uint8 *)Memory->PermanentStorage + sizeof(game_state),
                        Memory->PermanentStorageSize - sizeof(game_state));

        GameState->World = (world *)PushStruct(&GameState->WorldArena, world);
        world *World = GameState->World;
        World->TileMap = (tile_map *)PushStruct(&GameState->WorldArena, tile_map);

        tile_map *TileMap = World->TileMap;

        TileMap->ChunkShift = 4;
        TileMap->ChunkDim = (1 << TileMap->ChunkShift);
        TileMap->ChunkMask = ((1 << TileMap->ChunkShift) - 1); 

        TileMap->TileChunkCountX = 128;
        TileMap->TileChunkCountY = 128;
        TileMap->TileChunkCountZ = 2;

        int TileChunkCount = TileMap->TileChunkCountX * TileMap->TileChunkCountY * TileMap->TileChunkCountZ;
        TileMap->TileChunks = (tile_chunk *)PushArray(&GameState->WorldArena, TileChunkCount, tile_chunk);

        TileMap->TileSideInMeters = 1.4f;

        int32 RandomNumberIndex = 0;
        int32 TilePerWidth = 17;
        int32 TilesPerHeight = 9;

        int32 ScreenX = 0;
        int32 ScreenY = 0;
        int32 AbsTileZ = 0;


        // GameState->PlayerP.AbsTileX = 1;
        // GameState->PlayerP.AbsTileY = 3;
        // GameState->PlayerP.Offset.X = 5.0f;
        // GameState->PlayerP.Offset.Y = 5.0f;
        //
        // Input->Controllers[0].IsConnected = 1;


        bool32 DoorTop = 0;
        bool32 DoorBottom = 0;
        bool32 DoorLeft = 0;
        bool32 DoorRight = 0;
        bool32 StairsUp = 0;
        bool32 StairsDown = 0;

        for(int32 ScreenIndex = 0; ScreenIndex < 100; ScreenIndex++)
        {
            int32 RandomChoice;

            if(StairsUp || StairsDown) {
                RandomChoice = RandomNumberTable[RandomNumberIndex++] % 2;
            } else {
                RandomChoice = RandomNumberTable[RandomNumberIndex++] % 3;
            }

            if(RandomChoice == 2) {
                if(AbsTileZ == 0) {
                    StairsUp = true;
                } else {
                    StairsDown = true;
                }
            } else if(RandomChoice == 1) {
                DoorRight = true;
            } else {
                DoorTop = true;
            }
            
            for(int32 TileY = 0; TileY < TilesPerHeight; TileY++) 
            {
                for(int32 TileX = 0; TileX < TilePerWidth; TileX++) 
                {
                    int32 AbsTileX = ScreenX*TilePerWidth + TileX;
                    int32 AbsTileY = ScreenY*TilesPerHeight + TileY;

                    uint32 Value = 1;

                    if((TileX == 0) || (TileX == TilePerWidth - 1) || 
                       (TileY == 0) || (TileY == TilesPerHeight - 1)) 
                    {
                        Value = 2;
                    } 

                    if((StairsUp) && (TileY == 3) && (TileX == 3)) {
                        Value = 3;
                    }
                    if((StairsDown) && (TileY == 3) && (TileX == 3)) {
                        Value = 4;
                    }

                    if(DoorTop && (TileY == TilesPerHeight-1) && (TileX == TilePerWidth/2)) {
                        Value = 1; 
                    }
                    if(DoorBottom && (TileY == 0) && (TileX == TilePerWidth/2)) {
                        Value = 1; 
                    }

                    if(DoorRight && (TileX == TilePerWidth-1) && (TileY == TilesPerHeight/2)) {
                        Value = 1; 
                    }
                    if(DoorLeft && (TileX == 0) && (TileY == TilesPerHeight/2)) {
                        Value = 1; 
                    }

                    SetTileValue(&GameState->WorldArena, TileMap, AbsTileX, AbsTileY, AbsTileZ, Value);
                }
            }


            if(StairsUp && RandomChoice == 2) 
            {
                StairsDown = true;
                StairsUp = false;
            }
            else if(StairsDown && RandomChoice == 2) 
            {
                StairsDown = false;
                StairsUp = true;
            } else {
                StairsDown = false;
                StairsUp = false;
            }

            DoorLeft = DoorRight;
            DoorBottom = DoorTop;

            DoorRight = false;
            DoorTop = false;

            if(RandomChoice == 2) {
                if(AbsTileZ == 1) {
                    AbsTileZ = 0;
                } else {
                    AbsTileZ = 1;
                }
            }
            else if(RandomChoice == 1) {
                ScreenX += 1;
            }
            else if(RandomChoice == 0)
            {
                ScreenY += 1;
            }
        }

        {
            Input->Controllers[0].IsConnected = true;
            uint32 EntityIndex = AddEntity(GameState);
            InitializePlayer(GameState, EntityIndex);
            GameState->PlayerIndexForController[0] = EntityIndex;
        }

        Memory->IsInitialized = true;
    }

    world *World = GameState->World;
    tile_map *TileMap = World->TileMap;

    // real32 TileSideInPixels = 10;
    real32 TileSideInPixels = Buffer->Height/9;
    real32 MetersToPixels = TileSideInPixels / GameState->World->TileMap->TileSideInMeters;
    
    for(int ControllerIndex = 0;
            ControllerIndex < ArrayCount(Input->Controllers);
            ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input, ControllerIndex);
        entity *ControllingEntity = GetEntity(GameState,
                                              GameState->PlayerIndexForController[ControllerIndex]);
        if(ControllingEntity)
        {
            v2 ddP = {};

            if(Controller->IsAnalog)
            {
                // NOTE(casey): Use analog movement tuning
                ddP = v2{Controller->StickAverageX, Controller->StickAverageY};
            }
            else
            {
                // NOTE(casey): Use digital movement tuning
                if(Controller->ButtonTop.IsPressed)
                {
                    ddP.Y = 1.0f;
                }
                if(Controller->ButtonBottom.IsPressed)
                {
                    ddP.Y = -1.0f;
                }
                if(Controller->ButtonLeft.IsPressed)
                {
                    ddP.X = -1.0f;
                }
                if(Controller->ButtonRight.IsPressed)
                {
                    ddP.X = 1.0f;
                }
            }

            MovePlayer(GameState, ControllingEntity, Input->dtForFrame, ddP);
        }
        else
        {
            // if(Controller->Start.EndedDown)
            // {
            //     uint32 EntityIndex = AddEntity(GameState);
            //     InitializePlayer(GameState, EntityIndex);
            //     GameState->PlayerIndexForController[ControllerIndex] = EntityIndex;
            // }
        }
    }

    entity *CameraFollowingEntity = GetEntity(GameState, GameState->CameraFollowingEntityIndex);
    if(CameraFollowingEntity)
    {
        GameState->CameraP.AbsTileZ = CameraFollowingEntity->P.AbsTileZ;

        tile_map_difference Diff = Subtract(TileMap, &CameraFollowingEntity->P, &GameState->CameraP);
        if(Diff.dXY.X > (9.0f*TileMap->TileSideInMeters))
        {
            GameState->CameraP.AbsTileX += 17;
        }
        if(Diff.dXY.X < -(9.0f*TileMap->TileSideInMeters))
        {
            GameState->CameraP.AbsTileX -= 17;
        }
        if(Diff.dXY.Y > (5.0f*TileMap->TileSideInMeters))
        {
            GameState->CameraP.AbsTileY += 9;
        }
        if(Diff.dXY.Y < -(5.0f*TileMap->TileSideInMeters))
        {
            GameState->CameraP.AbsTileY -= 9;
        }
    }


    DrawBitmap(Buffer, &GameState->Backdrop, 0.0f, 0.0f);

    real32 ScreenCenterX = 0.5*(real32)Buffer->Width;
    real32 ScreenCenterY = 0.5*(real32)Buffer->Height;

    int32 RenderSize = 20;
    for(int32 Row = -RenderSize; Row <= RenderSize; Row++) {
        for(int32 Column = -RenderSize; Column <= RenderSize; Column++ ) {

            uint32 Color = 0xff000000;

            uint32 AbsTileX = GameState->CameraP.AbsTileX + Column;
            uint32 AbsTileY = GameState->CameraP.AbsTileY + Row;
            uint32 AbsTileZ = GameState->CameraP.AbsTileZ;

            uint32 Value = GetTileValue(GameState->World->TileMap, AbsTileX, AbsTileY, AbsTileZ);

            if(Value == 1) { Color = 0xffaaaaaa; }
            if(Value == 2) { Color = 0xff333333; } // NOTE: TestChange
            if(Value == 3) { Color = 0xff222222; }
            if(Value == 4) { Color = 0xffcccccc; }


            // real32 TileSideX = 0.5f * TileSideInPixels;
            // real32 TileSideY = 0.5f * TileSideInPixels;
            v2 HalfTileSide = {0.5f * TileSideInPixels, 0.5f * TileSideInPixels};

            // int32 CenX = ScreenCenterX + (Column * TileSideInPixels) - GameState->PlayerP.Offset.X * MetersToPixels;
            // int32 CenY = ScreenCenterY - (Row + 1) * GameState->World->TileMap->TileSideInMeters*MetersToPixels + GameState->PlayerP.Offset.Y*MetersToPixels;

            v2 Center = {
                ScreenCenterX - MetersToPixels*GameState->CameraP.Offset_.X + (real32)Column*TileSideInPixels,
                ScreenCenterY + MetersToPixels*GameState->CameraP.Offset_.Y - (real32)Row*TileSideInPixels,
            };

            // real32 X = CenX - TileSideX;
            // real32 Y = CenY + TileSideY;

            v2 Min = Center - HalfTileSide;
            v2 Max = Center + HalfTileSide;

            if(Value != 1) {
                real32 R = ((Color >> 16) & 0xFF) / 255.0f;
                real32 G = ((Color >> 8)  & 0xFF) / 255.0f;
                real32 B = ((Color >> 0)  & 0xFF) / 255.0f;

                DrawRectangle(Buffer, Min, Max, R, G, B);
            }
        }
    }

    entity *Entity = GameState->Entities;
    for(uint32 EntityIndex = 0;
        EntityIndex < GameState->EntityCount;
        ++EntityIndex, ++Entity)
    {
        // TODO(casey): Culling of entities based on Z / camera view
        if(Entity->Exists)
        {
            tile_map_difference Diff = Subtract(TileMap, &Entity->P, &GameState->CameraP);    

            // NOTE: TestChange
            real32 PlayerR = 0.318f;
            real32 PlayerG = 0.314f;
            real32 PlayerB = 0.309f;
            real32 PlayerGroundPointX = ScreenCenterX + MetersToPixels*Diff.dXY.X;
            real32 PlayerGroundPointY = ScreenCenterY - MetersToPixels*Diff.dXY.Y; 
            v2 PlayerLeftTop = {PlayerGroundPointX - 0.5f*MetersToPixels*Entity->Width,
                                PlayerGroundPointY - 0.5f*MetersToPixels*Entity->Height};
            v2 EntityWidthHeight = {Entity->Width, Entity->Height};
            DrawRectangle(Buffer,
                          PlayerLeftTop,
                          PlayerLeftTop + MetersToPixels*EntityWidthHeight,
                          PlayerR, PlayerG, PlayerB);

            hero_bitmaps *HeroBitmaps = &GameState->HeroBitmaps[Entity->FacingDirection];
            DrawBitmap(Buffer, &HeroBitmaps->Torso, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
            DrawBitmap(Buffer, &HeroBitmaps->Cape, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
            DrawBitmap(Buffer, &HeroBitmaps->Head, PlayerGroundPointX, PlayerGroundPointY, HeroBitmaps->AlignX, HeroBitmaps->AlignY);
        }
    }


    for(int ControllerIndex = 0;
            ControllerIndex < ArrayCount(Input->Controllers);
            ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input, ControllerIndex);
        if(Controller->IsConnected) {
            DrawButton(Buffer, Controller->ButtonRight);
            DrawButton(Buffer, Controller->ButtonLeft);
            DrawButton(Buffer, Controller->ButtonTop);
            DrawButton(Buffer, Controller->ButtonBottom);
        }
    }

}

// extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
// {
//     game_state *GameState = (game_state *)Memory->PermanentStorage;
//     GameOutputSound(GameState, SoundBuffer, 400);
// }

