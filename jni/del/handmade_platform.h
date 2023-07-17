#ifndef HANDMADE_PLATFORM_H
#define HANDMADE_PLATFORM_H

#include <stdint.h>

#define global_variable static
#define local_persist static
#define internal static


typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef float real32;
typedef float real64;

#define KILLOBYTE(value) (value * 1024L)
#define MEGABYTE(value) KILLOBYTE(value * 1024L)
#define GIGABYTE(value) MEGABYTE(value * 1024L)
#define TERRABYTE(value) GIGABYTE(value * 1024L)

#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))


#if HANDMADE_SLOW
#define Assert(Expression) if(!(Expression)) { *(volatile int *)0xdead = 0xbeef;}
// #define Assert(Expression) if(!Expression) { __builtin_trap();}
// #define Assert(Expression)
#else
#define Assert(Expression)
#endif



typedef struct {
    uint64 FileSize;
    uint8 *Contents;
} file_read_result;


#define DEBUG_READ_ENTIRE_FILE(name) file_read_result name(uint8 *Path)
typedef DEBUG_READ_ENTIRE_FILE(debug_platform_read_entire_file);


typedef struct  {
    bool32 IsInitialized;

    uint8 *PermanentStorage;
    uint64 PermanentStorageSize;

    uint8 *TransientStorage;
    uint64 TransientStorageSize;

    debug_platform_read_entire_file *DEBUGReadEntireFile;
} game_memory;


typedef struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16 *Samples;
} game_sound_output_buffer;


typedef struct {
    real32 PositionX;
    real32 PositionY;

    real32 Width;
    real32 Height;

    bool32 IsPressed;
} button;

// typedef struct {
//    real32 TouchX;
//    real32 TouchY;
//    bool32 IsTouching;
//
//     button ButtonRight;
//     button ButtonTop;
//     button ButtonBottom;
//     button ButtonLeft;
//
//     double dtForFrame;
// } game_input;

typedef struct {
    real32 TouchX;
    real32 TouchY;
    bool32 IsTouching;

    bool32 IsConnected;
    bool32 IsAnalog;

    real32 StickAverageX;
    real32 StickAverageY;

    button ButtonRight;
    button ButtonTop;
    button ButtonBottom;
    button ButtonLeft;

} game_controller_input;

typedef struct {
    double dtForFrame;
    game_controller_input Controllers[5];
} game_input;



game_controller_input *
GetController(game_input *Input, int Index) {
    Assert(ArrayCount(Input->Controllers) > Index);
    game_controller_input *Result = &Input->Controllers[Index];
    return Result;
}


#endif
