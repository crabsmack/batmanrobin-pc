#include "game.h"

/* Scr2_002D28: a script that is just the return op ($2D2A): a coroutine slot's idle value
   (Obj_InitSpawned's) */
static constexpr uint32_t Scr2_002D28 = 0x002D28u;

/* Op2_StopCoroutine0 - a2 op: stops coroutine slot 0 by pointing it back at the idle script
 * (just the return op), the slot's value from Obj_InitSpawned.
 * In:  -
 * Out: vm->coroutine[0] = Scr2_002D28; NEXT
 * Evidence: move.l #$2D28,$38(a4) / NEXT; the word at $2D28 is the return op $2D2A; 51 corpus
 *           runs incl. the TAS
 */
void Op2_StopCoroutine0(const u16 **script, Vm2Block *vm)
{
    vm->coroutine[0].v = Scr2_002D28;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
