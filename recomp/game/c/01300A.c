#include "game.h"

/* Drop_RandomHealthA_case4 - case 4 of Drop_RandomHealthA's jump table: bra.s Drop_HealthA1.
 * In:  as Drop_RandomHealthA
 * Out: as Drop_HealthA1
 * Evidence: bra.s Drop_HealthA1 at $01300A, the target of jmp $13006(pc,d0.w) with d0 = 4; never run in the corpus
 */
void Drop_RandomHealthA_case4(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4,
                              uint32_t *a6)
{
    dk::jump(at_Drop_HealthA1);
}
