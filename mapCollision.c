#include "mapCollision.h"

const int LevelTileSize = 16;

//
const uint8_t* LevelCollisionArray;
uint16_t LevelCollisionWidth;
uint16_t LevelCollisionHeight;

// const uint16_t LevelLevelTileSize = 16;
const uint16_t LevelCollisionShift = 4;

void SetMapCollision(const uint8_t* CollisionArray, const uint16_t Width, const uint16_t Height)
{
    LevelCollisionArray = CollisionArray;
    LevelCollisionWidth = Width;
    LevelCollisionHeight = Height;
}

uint8_t CheckMapCollision(const int16_t X, const int16_t Y)
{
    // What tile is it in? Calls the function found in the mapCollision.h file
    const uint16_t tx = X >> LevelCollisionShift;
    const uint16_t ty = Y >> LevelCollisionShift;
    register uint16_t tiletype = CheckMapCollisionTileFast(tx, ty);

    // Decide based on what the collision found
    switch(tiletype)
    {
        case TileBlank:         // If it was ~nothing~, return TileBlank
        case TileSolid:         // If it was a solid square, return here with TileSolid.
            return tiletype;
        default:            // If it was something else, don't return here.
        break;
    }

    // What pixel in the tile is it in? 
    uint16_t px = X % LevelTileSize;
    uint16_t py = Y % LevelTileSize;
    uint8_t hit;    // Contains the result of the collision check, 0 for nothing, 1 for something

    switch(tiletype)
    {
        // Ascending slopes
        case TileSlopeLR:       hit = (py > (LevelTileSize - px));    break;                      // 1:1 slope
        case TileSlopeLR2_1:    hit = ((2 * py) > (2 * LevelTileSize - px));    break;            // 2:1 slope
        case TileSlopeLR2_2:    hit = ((2 * py) > (2 * LevelTileSize - px - LevelTileSize));   break;
        // case TileSlopeLR3_1:    hit = ((3 * py) > (3 * LevelTileSize - px));    break;            // 3:1 slope
        // case TileSlopeLR3_2:    hit = ((3 * py) > (3 * LevelTileSize - px - LevelTileSize)); break;
        // case TileSlopeLR3_3:    hit = ((3 * py) > (3 * LevelTileSize - px - 2 * LevelTileSize)); break;

        // Descending slopes
        case TileSlopeRL:       hit = (py > px); break;                                      // 1:1 slope
        case TileSlopeRL2_1:    hit = ((2 * py) > px); break;                                // 2:1 slope
        case TileSlopeRL2_2:    hit = ((2 * py) > (px + LevelTileSize));    break;
        // case TileSlopeRL3_1:    hit = ((3 * py) > px); break;                                // 3:1 slope
        // case TileSlopeRL3_2:    hit = ((3 * py) > (px + LevelTileSize));  break;
        // case TileSlopeRL3_3:    hit = ((3 * py) > (px + 2 * LevelTileSize));  break;

        // Half blocks
        // case TileSolidTopHalf:      hit = ((2 * py) < LevelTileSize);     break;
        // case TileSolidBottomHalf:   hit = ((2 * py) > LevelTileSize);     break;
        // case TileSolidLeftHalf:     hit = ((px * 2) < LevelTileSize);     break;
        // case TileSolidRightHalf:    hit = ((px * 2) > LevelTileSize);     break;

        // Whatever it found, it wasn't in list, just return the value it found, treating it like a square block: 
        default:
            return tiletype;
        break;
    }

    /*
        if HIT didn't find anything, it will be 0.
        if HIT did find something, it will be 1.
        Multiply by tiletype found earlier, so if it's a slope tile (e.g. 8), the result will be 0 or 8 
    */

    return hit * tiletype;
}