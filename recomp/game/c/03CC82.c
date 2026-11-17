#include "game.h"

enum {
    REVEAL_MAP_BYTES = 0x6E0,   /* the cell map: 40 x 22 words in g_SceneWorkBuf */
};

/* Stage3_RevealInit - starts the cell-by-cell reveal of the clearing's plane-B picture: clears the
 * cell map, puts the cursor before its first cell, seeds the burst spot order at random and makes
 * the V-int object that sends the queued cells (Stage3_RevealVIntFlush)
 * In:  d6 = Mem_Clear's odd-length flag (passes through it)
 * Out: d0 = 0, or nonzero (VIntObj_New's -1) when no pool block was free for the V-int object;
 *      the reveal's cursor/column/row/seed/vint_obj (g_SceneVars+0..+8); d6/d7 = Mem_Clear's;
 *      a1/a6 = VIntObj_New's (a6 = the V-int object)
 * Evidence: move.w #$FFFF,(g_SceneVars+2).w / clr.w +4 / move.w a0,(g_SceneVars).w / Mem_Clear $6E0
 *           / Rand_Next -> +6 / lea Stage3_RevealVIntFlush,a0 / VIntObj_New / bne / move.w a6,+8 /
 *           moveq #0,d0; Op2_CallNativeRel at $03CBB8 in the director Scr2_03CB90; TAS f97888
 */
void Stage3_RevealInit(uint32_t *d0, uint16_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a6)
{
    ClearingReveal *rv = clearing_reveal();
    rv->column = -1;
    rv->row = 0;
    rv->cursor = (uint16_t)dk::addr(g_SceneWorkBuf);
    *d7 = REVEAL_MAP_BYTES;
    Mem_Clear(d6, d7, g_SceneWorkBuf);
    *d0 = (uint32_t)Rand_Next();
    rv->seed = (uint16_t)*d0;
    VIntObj_New(d0, at_Stage3_RevealVIntFlush, a1, a6);
    if (*d0 != 0)
        return;
    rv->vint_obj = (int16_t)(uint16_t)*a6;
    *d0 = 0;
}
