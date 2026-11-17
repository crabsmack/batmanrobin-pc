#include "game.h"

/* Obj_NewMulti_ClearRun ($00896C-$008971): the first 3 words of Obj_NewMulti's clear run, falling
 * into Obj_NewMulti_Clear4 (the next function file). */

/* Obj_NewMulti_ClearRun - the clear run's 1-block entry: zeroes 3 handle words, then
 * Obj_NewMulti_Clear4 zeroes 5 more and ends Obj_NewMulti.
 * In:  d0 = the clears' value (0; low word); a1 = the first unused handle word; a2 = the first
 *      block not taken
 * Out: as Obj_NewMulti_Clear4 leaves them
 * Evidence: 3 x move.w d0,(a1)+, then the next file; entered by jmp $896A(pc,d1.w) at $8968
 *           with d1 = 2 (1 block), and by Obj_NewMulti's first jump for 0 blocks
 */
void Obj_NewMulti_ClearRun(uint32_t *d0, uint32_t *a1, uint32_t a2)
{
    (void)a2;
    PTR(s16) p;
    p.v = *a1;
    s16 *slot = p;
    for (int i = 0; i < 3; i++)
        *slot++ = (int16_t)(uint16_t)*d0;
    *a1 = ADDR68(slot);
    dk::jump(at_Obj_NewMulti_Clear4);
}
