#include "game.h"

/* the throws (dx, dy, heading, kind x 4 words) and the puff's a4 script (ROM) */
static constexpr uint32_t Tbl_01DD48 = 0x01DD48u;
static constexpr uint32_t Scr4_01DDA0 = 0x01DDA0u;

static inline uint32_t low_word(uint32_t r, uint16_t v) { return (r & 0xFFFF0000u) | v; }
static inline uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }

/* Boss_JokerBalloon_ThrowByArg - balloon boss native: takes entry g_VM2Arg0 of the 8-byte table
 * Tbl_01DD48 (dx, dy, heading, kind), runs the puff a4 script Scr4_01DDA0 at the boss's x/y +
 * (dx, dy) (Fx_RunScript) and spawns that kind's thrown object there (Boss_JokerBalloon_SpawnThrown)
 * In:  a6 = the boss (Vm2Obj x/y); g_VM2Arg0's high word = the entry
 * Out: d2 = 0, d3 = the heading and d4/d5 = the position, swapped (16.16), d6 low word = the kind,
 *      as the table and the two calls leave them; d0/d1/d7/a0 = Boss_JokerBalloon_SpawnThrown's;
 *      a1-a6 put back (movem)
 * Evidence: movem.l a1-a6,-(a7) / move.w g_VM2Arg0,d0 / lsl.w #3 / lea Tbl_01DD48(pc,d0.w),a0 /
 *           moveq #0 to d2-d5 / move.w (a0)+ to d4, d5, d3, d6 / add.w $3E/$42(a6) / swap d3-d5 /
 *           movem.l d4-d6 around jsr Fx_RunScript with Scr4_01DDA0 / bsr Boss_JokerBalloon_SpawnThrown
 *           / movem.l (a7)+,a1-a6; Op2_CallNativeRel at $01D842 in the balloon boss's script; no
 *           corpus run reaches it (crane-crash 2026-10-30); 01DCFE.syn checks it. Prototype: no flags
 *           out (the caller's next op reads none)
 */
void Boss_JokerBalloon_ThrowByArg(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                  uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a1, uint32_t a2, uint32_t a3,
                                  uint32_t a4, uint32_t a5, uint32_t a6)
{
    PTR(Vm2Obj) boss;
    boss.v = a6;
    uint16_t entry = (uint16_t)(g_VM2Arg0 >> 16);          /* move.w: the long's high word */
    *d0 = low_word(*d0, (uint16_t)(entry << 3));
    PTR(const s16) throw_;
    throw_.v = Tbl_01DD48 + (uint32_t)(int32_t)(int16_t)(uint16_t)*d0;
    const s16 *w = throw_;
    *d2 = 0;
    *d3 = (uint16_t)w[2];
    *d4 = (uint16_t)w[0];
    *d5 = (uint16_t)w[1];
    *d6 = low_word(*d6, (uint16_t)w[3]);
    *a0 = throw_.v + 8;
    *d4 = low_word(*d4, (uint16_t)((uint16_t)*d4 + (uint16_t)boss->x));
    *d5 = low_word(*d5, (uint16_t)((uint16_t)*d5 + (uint16_t)boss->y));
    *d3 = swap(*d3);
    *d4 = swap(*d4);
    *d5 = swap(*d5);
    uint32_t s4 = *d4, s5 = *d5, s6 = *d6;                /* movem.l d4-d6 around the puff */
    *a0 = Scr4_01DDA0;
    Fx_RunScript(d0, d1, d2, d3, d4, d5, d6, d7, *a0, a1, a2, a3, a4, a5, a6);
    *d4 = s4;
    *d5 = s5;
    *d6 = s6;
    uint32_t r3 = a3, r4 = a4, r5 = a5, r6 = a6;          /* movem.l (a7)+,a1-a6 puts them back */
    uint16_t lo1 = (uint16_t)*d1;
    Boss_JokerBalloon_SpawnThrown(d0, &lo1, *d4, *d5, (uint16_t)*d6, d7, a0, &r3, &r4, &r5, &r6);
    *d1 = low_word(*d1, lo1);
}
