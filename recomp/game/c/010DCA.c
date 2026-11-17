#include "game.h"

/* The terrain dispatch and the block-map / no-terrain routines ($010DCA-$010E09). */

/* Coll_MoveScene - moves a box through the scene's terrain: jumps to g_TerrainFn, or to
 * g_TerrainFnAlt when bit 0 of the motion block's flags is set (grounded movers).
 * In:  mot = the motion block (a3); d1/d2 = the move, d4/d5 = the box position, d6/d7 = the masks
 *      (handed on untouched)
 * Out: a0 = the routine; d3-d7 as it leaves them (the prototype passes every register: a C caller,
 *      Op2_MoveTerrain, reads the routine's results)
 * Evidence: btst #0,(a3) / bne / movea.l (g_TerrainFn).w,a0 / jmp (a0) / movea.l (g_TerrainFnAlt).w,a0
 *           / jmp (a0); called by the a2 move op $4CDA and $15730
 */
void Coll_MoveScene(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, const Vm2Motion *mot, uint32_t *a4,
                    uint32_t *a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a1, (void)a2, (void)a4, (void)a5, (void)a6;
    *a0 = (mot->flags & 1) ? (uint32_t)g_TerrainFnAlt : (uint32_t)g_TerrainFn;
    dk::jump(dk::code(*a0));
}

/* Coll_MoveOnBlockMap - terrain routine of the plane-A block map: Coll_WalkBlockMap (following
 * the floor) when bit 0 of the motion block's flags is set, else Coll_MoveAirborneBlockMap; either
 * with d1-d2/a0-a6 preserved.
 * In:  as Coll_MoveScene
 * Out: d0/d3-d7 from the block-map routine
 * Evidence: btst #0,(a3) / bne / movem.l d1-d2/a0-a6,-(a7) / bsr.w Coll_WalkBlockMap / movem.l
 *           (a7)+ / rts, the airborne side falling into Coll_MoveAirborneBlockMap; jsr from $24FAC
 */
void Coll_MoveOnBlockMap(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2, const Vm2Motion *mot,
                         uint32_t a4, uint32_t a5, uint32_t a6)
{
    if (!(mot->flags & 1))
        Coll_MoveAirborneBlockMap(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, mot, a4, a5, a6);
    else
        Coll_MoveGroundedBlockMap(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, mot, a4, a5, a6);
}

/* Coll_MoveGroundedBlockMap - the grounded side of Coll_MoveOnBlockMap without its bit test:
 * Coll_WalkBlockMap (following the floor) with d1-d2/a0-a6 preserved.
 * In:  as Coll_MoveScene
 * Out: d0/d3-d7 from Coll_WalkBlockMap
 * Evidence: movem.l d1-d2/a0-a6,-(a7) / bsr.w Coll_WalkBlockMap / movem.l (a7)+,d1-d2/a0-a6 / rts;
 *           a terrain routine pointer in the scene scripts ($01FBFE, $01FD1A, $01FE2A, $03FE3C,
 *           $04004C), entered by the stepped scenario-level-skip (sync 1,188)
 */
void Coll_MoveGroundedBlockMap(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2,
                               const Vm2Motion *mot, uint32_t a4, uint32_t a5, uint32_t a6)
{
    (void)d2, (void)a0, (void)a1, (void)a2, (void)mot, (void)a4, (void)a5, (void)a6;
    uint16_t attr = (uint16_t)*d3;
    Coll_WalkBlockMap(d0, (uint16_t)d1, &attr, d4, d5, d6, d7);
    *d3 = (*d3 & 0xFFFF0000u) | attr;
}

/* Coll_MoveAirborneBlockMap - the airborne side of Coll_MoveOnBlockMap without its bit test:
 * Coll_MoveBlockMap (pixel steps) with d1-d2/a0-a6 preserved (its other outputs are dropped, as
 * the movem restores them).
 * In:  as Coll_MoveScene
 * Out: d0/d3-d7 from Coll_MoveBlockMap
 * Evidence: movem.l d1-d2/a0-a6,-(a7) / bsr.w Coll_MoveBlockMap / movem.l (a7)+,d1-d2/a0-a6 / rts; a
 *           runtime entry of the reference ($010DE2: a computed jump reached it, a terrain routine
 *           pointer in scene data)
 */
void Coll_MoveAirborneBlockMap(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t a1, uint32_t a2,
                               const Vm2Motion *mot, uint32_t a4, uint32_t a5, uint32_t a6)
{
    uint16_t dx = (uint16_t)d1, dy = (uint16_t)d2;
    uint32_t r[7] = { a0, a1, a2, dk::addr(mot), a4, a5, a6 };
    Coll_MoveBlockMap(d0, &dx, &dy, d3, d4, d5, d6, d7, &r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6]);
}

/* Coll_NoTerrain - terrain routine of the scenes without ground: the move applies, no contact.
 * In:  d1/d2 = the move; d4/d5 = the position
 * Out: d4/d5 moved; d6 = d7 = 0
 * Evidence: add.w d1,d4 / add.w d2,d5 / moveq #0,d6 / moveq #0,d7; terrain routine pair in the scene
 *           scripts at $401AA, $4798A, $47B78
 */
void Coll_NoTerrain(uint16_t d1, uint16_t d2, uint16_t *d4, uint16_t *d5, uint32_t *d6, uint32_t *d7)
{
    *d4 = (uint16_t)(*d4 + d1);
    *d5 = (uint16_t)(*d5 + d2);
    *d6 = 0;
    *d7 = 0;
}

/* Coll_ScriptSetFlatFloor - native op: Coll_SetFlatFloor at the height in the next script word.
 * In:  script = the operand (a2: word floor y)
 * Out: script past the word; d0 low word = the height; then Coll_SetFlatFloor
 * Evidence: move.w (a2)+,d0 and the fall into Coll_SetFlatFloor; native callback (a2 op $2CBC)
 *           at $1C4FC
 */
void Coll_ScriptSetFlatFloor(uint16_t *d0, const u16 **script)
{
    *d0 = *(*script)++;
    dk::jump(at_Coll_SetFlatFloor);
}
