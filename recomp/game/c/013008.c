#include "game.h"

/* Drop_RandomHealthA_case2 - case 2 of Drop_RandomHealthA's jump table: bra.s Drop_HealthA2.
 * In:  as Drop_RandomHealthA
 * Out: as Drop_HealthA2
 * Evidence: bra.s Drop_HealthA2 at $013008, the target of jmp $13006(pc,d0.w) with d0 = 2; never run in the corpus
 */
void Drop_RandomHealthA_case2(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4,
                              uint32_t *a6)
{
    dk::jump(at_Drop_HealthA2);
}
