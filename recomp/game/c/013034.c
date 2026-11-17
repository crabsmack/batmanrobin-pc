#include "game.h"

/* Drop_RandomHealthB_case4 - case 4 of Drop_RandomHealthB's jump table: one B health pickup (the
 * table has B1 twice, so one pickup comes up half the time).
 * In:  as Drop_RandomHealthB
 * Out: as Drop_HealthB1
 * Evidence: bra.s Drop_HealthB1 at $013034, the target of jmp $13030(pc,d0.w) with d0 = 4 (11
 *           computed jumps in the call-site log)
 */
void Drop_RandomHealthB_case4(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *a4,
                              uint32_t *a6)
{
    dk::jump(at_Drop_HealthB1);
}
