#pragma once

#include <genesis.h>

extern const uint8_t* LevelCollisionArray;    // Pointer to a const uint8_t array, not a const pointer to a uint8_t array.
extern uint16_t LevelCollisionWidth;
extern uint16_t LevelCollisionHeight;

enum CollisionType
{
    // Basic
    TileBlank = 0, 
    TileSolid = 1, 
    TileJumpthrough = 2,
    // Slopes
    TileSlopeLR = 3, TileSlopeRL = 4,
    TileSlopeLR2_1 = 5, TileSlopeLR2_2 = 6,
    TileSlopeRL2_1 = 7, TileSlopeRL2_2 = 8,

    // other tiles
    ConveyorTileLeft = 9, ConveyorTileRight = 10,
    TileHurt = 11, TileDie = 12

    // unused from old system
    // TileSlopeLR3_1 = 8, TileSlopeLR3_2 = 9, TileSlopeLR3_3 = 10,
    // TileSlopeRL3_1 = 13, TileSlopeRL3_2 = 14, TileSlopeRL3_3 = 15
    
};

//

/// @brief Sets up collision data for current game map
/// @param CollisionArray 
/// @param Width 
/// @param Height 
void SetMapCollision(const uint8_t* CollisionArray, const uint16_t Width, const uint16_t Height);

/// @brief Check against the collision array, returns a uint8_t / u8 byte containing a CollisionType value
/// @param X 
/// @param Y 
/// @return 
uint8_t CheckMapCollision(const int16_t X, const int16_t Y);


/// @brief Check against the collision array using a tile index as fast as possible.
/// @param TX 
/// @param TY 
/// @return 
static inline uint8_t CheckMapCollisionTileFast(const int16_t TX, const int16_t TY)
{
#if defined(__GNUC__) && defined(__m68k__)
    uint8_t result;
    asm volatile
    (
        "move.w %[LevelCollisionWidth], %%d0      \n" // Width
        "mulu.w %[TY], %%d0                      \n"  // TY * Width
        "add.w %[TX], %%d0                       \n"  // + TX
        "move.l %[LevelCollisionArray], %%a0     \n"  // LevelCollisionArray[result]
        "move.b (%%a0, %%d0.w), %[result]        \n"
        : [result] "=r" (result)                 
        : [TX] "d" (TX), [TY] "d" (TY), [LevelCollisionWidth] "m" (LevelCollisionWidth),
          [LevelCollisionArray] "m" (LevelCollisionArray) 
        : "d0", "a0"                             
    );
    return result;
#else
    return LevelCollisionArray[TX + (TY * LevelCollisionWidth)];
#endif
}