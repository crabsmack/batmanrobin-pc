#include "game.h"

/* Drop_RandomHealthB_case6 - case 6 of Drop_RandomHealthB's jump table: bra.s Drop_HealthB3.
 * In:  as Drop_RandomHealthB
 * Out: as Drop_HealthB3
 * Evidence: bra.s Drop_HealthB3 at $013036, the target of jmp $13030(pc,d0.w) with d0 = 6; never run in the corpus
 */
void Drop_RandomHealthB_case6(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4,
                              uint32_t *a6)
{
    dk::jump(at_Drop_HealthB3);
}
