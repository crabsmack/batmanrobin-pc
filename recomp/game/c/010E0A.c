#include "game.h"

/* The flat-floor terrain and the two fixed-shape scene floors ($010E0A-$010F37). Every terrain
 * routine: d1/d2 = the move, d4/d5 = the box position (words), d7 = the mask of floors it may land
 * on; out d4/d5 moved, d3 = d6 = 0, d7 = the floor it landed on (0 = none). The edge tests are
 * signed word compares. */

/* Coll_SetFlatFloor - makes the scene terrain a flat floor at height d0: Coll_FlatFloor /
 * Coll_FlatFloorRest become the terrain routines.
 * In:  floor_y = the height (d0)
 * Out: g_FloorY, g_TerrainFn, g_TerrainFnAlt; the flags of the last move.l (all clear)
 * Evidence: move.w d0,(g_FloorY).w / move.l #$10E20,(g_TerrainFn).w / move.l #$10E46,(g_TerrainFnAlt).w;
 *           called from $1EE24 and through Coll_ScriptSetFlatFloor
 */
void Coll_SetFlatFloor(uint16_t floor_y, dk::Flags *ccr)
{
    g_FloorY = floor_y;
    g_TerrainFn = at_Coll_FlatFloor;
    g_TerrainFnAlt = at_Coll_FlatFloorRest;
    *ccr = dk::Flags::test(at_Coll_FlatFloorRest, 4, ccr->x);
}

/* Coll_FlatFloor - terrain routine for a flat floor at g_FloorY: lands (d7 = 1, y = the floor) only
 * when the move crosses from at or above the floor to below it and mask bit 0 is set.
 * In:  d1/d2 = the move; d4/d5 = the position; d7 = the mask (bit 0 the floor)
 * Out: d0 = g_FloorY; d4/d5; d3 = d6 = 0; d7 = 1 landed, else 0
 * Evidence: move.w (g_FloorY).w,d0 / add.w d1,d4 / moveq #0,d3 / moveq #0,d6 / btst #0,d7 / cmp.w
 *           d0,d5 / bgt / add.w d2,d5 / cmp.w d0,d5 / ble / move.w d0,d5 / moveq #1,d7; installed by
 *           Coll_SetFlatFloor and the scene scripts ($32E3E, $3FF5A ...)
 */
void Coll_FlatFloor(uint16_t *d0, uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6,
                    uint32_t *d7)
{
    int16_t floor_y = (int16_t)(uint16_t)g_FloorY;
    *d0 = (uint16_t)floor_y;
    *d4 = (uint16_t)(*d4 + d1);
    *d3 = 0;
    *d6 = 0;
    bool above = (*d7 & 1) && (int16_t)*d5 <= floor_y;
    *d5 = (uint16_t)(*d5 + d2);
    if (above && (int16_t)*d5 > floor_y) {
        *d5 = (uint16_t)floor_y;
        *d7 = 1;
        return;
    }
    *d7 = 0;
}

/* Coll_FlatFloorRest - as Coll_FlatFloor but a mover resting on the floor stays in contact: lands
 * when the new y is at least floor - 1.
 * In:  as Coll_FlatFloor
 * Out: as Coll_FlatFloor, except that a landing sets only d7's low word (move.w #1); d0 = floor - 1
 *      when the move ended above it
 * Evidence: as Coll_FlatFloor with subq.w #1,d0 / blt / addq.w #1,d0 / move.w #1,d7; installed as
 *           g_TerrainFnAlt beside Coll_FlatFloor
 */
void Coll_FlatFloorRest(uint16_t *d0, uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                        uint32_t *d6, uint32_t *d7)
{
    int16_t floor_y = (int16_t)(uint16_t)g_FloorY;
    *d0 = (uint16_t)floor_y;
    *d4 = (uint16_t)(*d4 + d1);
    *d3 = 0;
    *d6 = 0;
    bool above = (*d7 & 1) && (int16_t)*d5 <= floor_y;
    *d5 = (uint16_t)(*d5 + d2);
    if (!above) {
        *d7 = 0;
        return;
    }
    *d0 = (uint16_t)(floor_y - 1);
    if ((int16_t)*d5 < (int16_t)*d0) {
        *d7 = 0;
        return;
    }
    *d0 = (uint16_t)floor_y;
    *d5 = (uint16_t)floor_y;
    *d7 = (*d7 & 0xFFFF0000u) | 1;
}

/* the street floors: a landing on `level` (y) reports `which` in d7's low word */
static bool land(uint16_t d2, uint16_t *d5, int16_t level, uint32_t which, uint32_t *d3, uint32_t *d6, uint32_t *d7)
{
    *d5 = (uint16_t)(*d5 + d2);
    if ((int16_t)*d5 < level)
        return false;
    *d5 = (uint16_t)level;
    *d3 = 0;
    *d6 = 0;
    *d7 = (*d7 & 0xFFFF0000u) | which;
    return true;
}

/* Coll_Stage1StreetFloors - terrain routine of the stage 1 opening street: the street at y $C8
 * (mask bit 0) below y $6C; above it the walkways at y $60 (bit 1) and $6C (bit 2), there only
 * where x * (1 + 1/8 - 1/32) mod 360 is in $10-$150 (the gaps between the walkway pieces).
 * In:  d1/d2 = the move; d4/d5 = the position; d7 = the mask
 * Out: d4/d5; d3 = d6 = 0; d7 = 1 the street, 2/4 a walkway, else 0; d0 = the scaled x's
 *      quotient (high word) and remainder (low word) by 360 when the walkway test ran
 * Evidence: add.w d1,d4 / cmpi.w #$6C,d5 / bgt (the street) / moveq #0,d0 / move.w d4,d0 / lsr.w
 *           #3 / add.w / lsr.w #2 / sub.w / divu.w #$168,d0 / swap d0 / cmpi.w #$150 / #$10, the
 *           bit 1 / bit 2 tests and the landings (move.w #2/#4/#1,d7); scene table $1FAA6
 */
void Coll_Stage1StreetFloors(uint32_t *d0, uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                             uint32_t *d6, uint32_t *d7)
{
    *d4 = (uint16_t)(*d4 + d1);
    if ((int16_t)*d5 > 0x6C) {
        if ((*d7 & 1) && (int16_t)*d5 <= 0xC8) {
            if (land(d2, d5, 0xC8, 1, d3, d6, d7))
                return;
        } else {
            *d5 = (uint16_t)(*d5 + d2);
        }
    } else {
        uint16_t x = *d4;
        uint16_t eighth = (uint16_t)(x >> 3);
        uint16_t scaled = (uint16_t)(x + eighth - (eighth >> 2));
        uint16_t q = (uint16_t)(scaled / 0x168), rem = (uint16_t)(scaled % 0x168);
        *d0 = ((uint32_t)q << 16) | rem;
        *d3 = (*d3 & 0xFFFF0000u) | (uint16_t)(eighth >> 2);
        bool over_walkway = (int16_t)rem <= 0x150 && (int16_t)rem >= 0x10;
        if (over_walkway && (*d7 & 2) && (int16_t)*d5 <= 0x60) {
            if (land(d2, d5, 0x60, 2, d3, d6, d7))
                return;
        } else if (over_walkway && (*d7 & 4)) {
            if (land(d2, d5, 0x6C, 4, d3, d6, d7))
                return;
        } else {
            *d5 = (uint16_t)(*d5 + d2);
        }
    }
    *d3 = 0;
    *d6 = 0;
    *d7 = 0;
}

/* Coll_HatterArenaFloor - terrain routine of the Mad Hatter boss arena: a floor at y $D4 only for
 * x in $94-$206 (mask bit 0); elsewhere the mover falls.
 * In:  d1/d2 = the move; d4/d5 = the position; d7 = the mask
 * Out: d4/d5; d3 = d6 = 0; d7 = 1 landed, else 0
 * Evidence: moveq #0,d3 / moveq #0,d6 / add.w d1,d4 / cmpi.w #$94,d4 / blt / cmpi.w #$206,d4 / bgt /
 *           btst #0,d7 / cmpi.w #$D4,d5 / bgt / add.w d2,d5 / cmpi.w #$D4,d5 / blt / moveq #1,d7;
 *           the scene script at $40514
 */
void Coll_HatterArenaFloor(uint16_t d1, uint16_t d2, uint32_t *d3, uint16_t *d4, uint16_t *d5, uint32_t *d6,
                           uint32_t *d7)
{
    *d3 = 0;
    *d6 = 0;
    *d4 = (uint16_t)(*d4 + d1);
    int16_t x = (int16_t)*d4;
    bool over = x >= 0x94 && x <= 0x206 && (*d7 & 1) && (int16_t)*d5 <= 0xD4;
    *d5 = (uint16_t)(*d5 + d2);
    if (over && (int16_t)*d5 >= 0xD4) {
        *d5 = 0xD4;
        *d7 = 1;
        return;
    }
    *d7 = 0;
}
