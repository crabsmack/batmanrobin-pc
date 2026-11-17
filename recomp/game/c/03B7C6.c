#include "game.h"

enum { EYE_Y = -0xF };   /* the eyes' line, from the head's y */

static uint32_t swap32(uint32_t v) { return (v << 16) | (v >> 16); }

/* Boss_CatHead_EyeFxAt - the four cat-head eye natives' shared tail: the eye point (d0 + the head's x,
 * -$F + its y) through the arena's sway, a heading of the head's + the shear x 256 + the script's word,
 * then Fx_RunScript with the a4 script in a0
 * In:  d0's low word = the eye's x offset (-$1E / +$1E), a0 = the a4 script; a6 = the head (x +$3E,
 *      y +$42), a3 = its motion block (+$C heading), a2 = the Op2_CallNativeRel operand (word: the
 *      heading offset)
 * Out: a2 past the word; d0/d1 = the eye point, d2 = 0, d3 = the heading (16.16), d4/d5 = the
 *      position (16.16, Boss_CatHead_SwayPoint's swapped); then Fx_RunScript's results
 * Evidence: move.w #$FFF1,d1 / add.w $3E(a6),d0 / add.w $42(a6),d1 / bsr.s Boss_CatHead_SwayPoint /
 *           swap d4 / swap d5 / moveq #0,d2 / move.w (g_SceneVars+$10).w,d3 / ext.l / asl.l #8 /
 *           add.l $C(a3),d3 / swap / add.w (a2)+,d3 / swap / jmp Fx_RunScript; the bra.s of
 *           Boss_CatHead_EyeSparkLeft / Right and the fall-through of Boss_CatHead_EyeShotAt; no corpus
 *           run reaches it (raw-stubs 2026-10-30); 03B7C6.syn checks it
 */
void Boss_CatHead_EyeFxAt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t d6, uint32_t d7, uint32_t *a0, uint32_t a1, uint32_t *a2, uint32_t a3,
                          uint32_t a4, uint32_t a5, uint32_t a6)
{
    PTR(const Vm2Obj) self;
    self.v = a6;
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)EYE_Y;                                     /* move.w #$FFF1,d1 */
    *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d0 + (uint16_t)self->x);
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d1 + (uint16_t)self->y);
    Boss_CatHead_SwayPoint(*d0, *d1, d2, d3, d4, d5);
    *d4 = swap32(*d4);
    *d5 = swap32(*d5);
    *d2 = 0;                                                                         /* moveq #0,d2 */
    uint32_t h = (uint32_t)(int32_t)cat_head_arena()->shear << 8;                    /* ext.l / asl.l #8 */
    PTR(const Vm2Motion) motion;
    motion.v = a3;
    h = swap32(h + (uint32_t)motion->heading);
    PTR(const u16) arg;
    arg.v = *a2;
    h = (h & 0xFFFF0000u) | (uint16_t)((uint16_t)h + *arg);                         /* add.w (a2)+,d3 */
    *a2 += 2;
    *d3 = swap32(h);
    dk::jump(at_Fx_RunScript);
}
