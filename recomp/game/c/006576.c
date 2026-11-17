#include "game.h"

/* Op4_AltTerrainMoveOrFall - a4 op: moves the object's box from its last drawn point to d4 / d5
 * against the alternative terrain (g_TerrainFnAlt, wall mask 9, floor mask $B) and branches when
 * there is no floor contact (falling); on a floor it continues.
 * In:  a4 = script: word branch offset; d4 / d5 = the drawn x / y words; a6 = the object
 * Out: d4 / d5 = the corrected position, also the drawn point; the terrain routine's registers;
 *      the branch (Op4_Goto) or NEXT after the offset
 * Evidence: move.w #9,d6 / move.w #$B,d7 / move.w d4,d1 / move.w d5,d2 / movem.w $56(a6),d4-d5 /
 *           sub.w / movea.l (g_TerrainFnAlt).w,a0 / jsr (a0) / movem.w d4-d5,$56(a6) / tst.w d7 /
 *           beq.w Op4_Goto; script use $26D64 (`6576 0002 61FC`: then Op4_SetXYFromDraw); 3 corpus
 *           runs. Prototype: every register (was d0-d7 / a0 / a4 / a6): the terrain routine is
 *           called through the Op4TerrainFn type
 */
void Op4_AltTerrainMoveOrFall(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                              uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                              uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(Vm4Obj) self;
    self.v = *a6;
    *d6 = (*d6 & 0xFFFF0000u) | 9;
    *d7 = (*d7 & 0xFFFF0000u) | 0xB;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)*d4;
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)*d5;
    *d4 = (uint32_t)(int32_t)self->draw_x;   /* movem.w: sign-extended */
    *d5 = (uint32_t)(int32_t)self->draw_y;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)(*d1 - *d4);
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)(*d2 - *d5);
    *a0 = g_TerrainFnAlt;
    Op4TerrainFn(dk::code(*a0), d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
    self.v = *a6;
    self->draw_x = (int16_t)(uint16_t)*d4;
    self->draw_y = (int16_t)(uint16_t)*d5;
    if ((uint16_t)*d7 == 0) {   /* no floor: falling */
        dk::jump(at_Op4_Goto);
        return;
    }
    PTR(const u16) script;
    script.v = *a4 + 2;
    *a0 = dk::code((int16_t)(uint16_t)*script);
    *a4 += 4;
    dk::jump(*a0);
}
