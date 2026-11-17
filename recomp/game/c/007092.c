#include "game.h"

/* VM6_StartFromTrigger - stage trigger callback: starts the a6 loader script the trigger entry's
 * argument long names (a section's graphics load while the stage scrolls).
 * In:  a1 -> the long script address (the trigger entry's argument, past the callback long)
 * Out: a1 past the long, a6 = the script; falls into VM6_Start (the next file: d0 = 0, Z set =
 *      the trigger is done)
 * Evidence: movea.l (a1)+,a6 then VM6_Start's code; dc.l $7092 in the trigger tables at $21D8C
 *           (script $33050), $21D98 (script $3308A), $35598; TAS f46235-97872, 1 run
 */
void VM6_StartFromTrigger(uint32_t *a1, uint32_t *a6)
{
    PTR(const u32) arg;
    arg.v = *a1;
    *a6 = *arg;
    *a1 += 4;
    dk::jump(at_VM6_Start);
}
