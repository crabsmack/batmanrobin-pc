#include "game.h"

/* Drop_RandomHealthA - spawn action 48: when the random cooldown g_DropCooldown has run out,
 * re-arms it at random and drops one, two, one or three A health pickups (random too).
 * In:  x/y = the position (d4/d5, passed on); g_DropCooldown
 * Out: g_DropCooldown - 1, or a new random word; then (re-armed) d1 = random & 7, d0 = the case
 *      offset (random bits 9-10 x 2) and a jump to Drop_HealthA1/A2/A1/A3; still cooling down: the
 *      flags of the subq (callers' ccout)
 * Evidence: Drop_RandomHealthB's instructions with the A table: subq.w #1,(g_DropCooldown).w / bpl
 *           rts / jsr Rand_Next / move.w d0,d1 / andi.w #7,d1 / move.w d0,(g_DropCooldown).w /
 *           lsr.w #8,d0 / andi.w #6,d0 / jmp $13006(pc,d0.w); Tbl_012EF0 entry 48; never run in
 *           the corpus. Prototype: Drop_RandomHealthB's (the same code)
 */
void Drop_RandomHealthA(uint32_t *d0, uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                        uint32_t *a0, uint32_t *a4, uint32_t *a6, dk::Flags *ccr)
{
    uint16_t left = g_DropCooldown;
    g_DropCooldown = (uint16_t)(left - 1);
    dk::Flags sub = dk::Flags::compare(left, 1, 2);
    sub.x = sub.c;
    if (!sub.n) {                            /* bpl: still cooling down */
        *ccr = sub;
        return;
    }
    *d0 = (uint32_t)Rand_Next();
    *d1 = (uint16_t)(*d0 & 7);
    g_DropCooldown = (uint16_t)*d0;
    uint16_t offset = (uint16_t)(((uint16_t)*d0 >> 8) & 6);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    switch (offset) {                        /* the bra.s table at $013006 */
    case 0: dk::jump(at_Drop_RandomHealthA_case0); return;
    case 2: dk::jump(at_Drop_RandomHealthA_case2); return;
    case 4: dk::jump(at_Drop_RandomHealthA_case4); return;
    default: dk::jump(at_Drop_RandomHealthA_case6); return;
    }
}
