#include "game.h"

/* Hitbox_SpawnAtContact - spawns the a4 effect script (a hit spark) at the centre of the overlap
 * between the struck box and the attack rectangle, on g_SpawnCtx with radius and heading 0. The
 * caller's d3-d7/a0/a2 come back unchanged (movem); d0-d2 don't.
 * In:  script = the a4 script (a1); box = the struck box (a2); d4/d6, d5/d7 = the attack box's x
 *      and y edges; the rest passed on to Fx_RunScript
 * Out: d0-d2 as the effect script's first run left them (set up as the box's left/top from
 *      Hitbox_OverlapCenter and 0)
 * Evidence: movem.l d3-d7/a0/a2,-(a7) / movea.l a1,a0 / bsr Hitbox_OverlapCenter / moveq #0,d3 /
 *           moveq #0,d2 / swap d4 / swap d5 / jsr Fx_RunScript / movem.l (a7)+ / rts; branched to
 *           by every hit callback with a spark
 */
void Hitbox_SpawnAtContact(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5,
                           uint32_t d6, uint32_t d7, uint32_t a0, uint32_t script, const HitBox *box, uint32_t a3,
                           uint32_t *a4, uint32_t a5, uint32_t *a6)
{
    (void)d3;
    (void)a0;
    uint16_t x = (uint16_t)d4, y = (uint16_t)d5, x2 = (uint16_t)d6, y2 = (uint16_t)d7;
    Hitbox_OverlapCenter(d0, d1, &x, &y, &x2, &y2, box);
    *d2 = 0;
    /* swap: the centre becomes the integer part (high word) of the 16.16 position, the old high
       word its fraction */
    uint32_t cx = ((uint32_t)x << 16) | (d4 >> 16);
    uint32_t cy = ((uint32_t)y << 16) | (d5 >> 16);
    d6 = (d6 & 0xFFFF0000u) | x2;
    d7 = (d7 & 0xFFFF0000u) | y2;
    /* the script run changes the data registers (Fx_RunScript's prototype hands them back); the
       movem restores d3-d7 here, so only d0-d2 reach the caller */
    d3 = 0;
    Fx_RunScript(d0, d1, d2, &d3, &cx, &cy, &d6, &d7, script, script, dk::addr(box), a3, *a4, a5, *a6);
}
