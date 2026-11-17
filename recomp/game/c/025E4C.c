#include "game.h"

/* swap: the 68000's halves exchanged */
static uint32_t FxHull_Swap(uint32_t v)
{
    return (v << 16) | (v >> 16);
}

/* Boss_Stage2_FxAtHullPoint - boss native: runs the a4 effect script named by the dc.l after two
 * inline words (a point of the hull picture) at that point, tilted with the arena floor
 * In:  script = inline x, y words, then the dc.l effect script (a2)
 * Out: d0.w/d1.w = the point; d2 = d3 = 0; d4/d5 = the tilted point, 16.16 (Boss_Stage2_TiltPoint's
 *      words swapped); then Fx_RunScriptArg (which reads the dc.l and moves script past it)
 * Evidence: move.w (a2)+,d0 / move.w (a2)+,d1 / bsr Boss_Stage2_TiltPoint / moveq #0,d2 / moveq
 *           #0,d3 / swap d4 / swap d5 / jmp Fx_RunScriptArg; op $2CB2 at $025DE0 ($100,$5A,
 *           Scr4_025EE0), $025E30 and $025E3E in the defeat script; TAS f34683-f35412 (142 frames)
 */
void Boss_Stage2_FxAtHullPoint(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                               uint32_t d6, uint32_t d7, uint32_t a1, const u16 **script, uint32_t a3, uint32_t a4,
                               uint32_t a5, uint32_t a6)
{
    uint16_t px = *(*script)++;
    uint16_t py = *(*script)++;
    *d0 = (*d0 & 0xFFFF0000u) | px;
    *d1 = (*d1 & 0xFFFF0000u) | py;
    uint16_t slope = (uint16_t)*d2;
    Boss_Stage2_TiltPoint(px, py, &slope, d3, d4, d5);
    *d2 = 0;
    *d3 = 0;
    *d4 = FxHull_Swap(*d4);
    *d5 = FxHull_Swap(*d5);
    dk::jump(at_Fx_RunScriptArg);
}
