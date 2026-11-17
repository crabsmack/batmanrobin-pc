#include "game.h"

/* Enemy_PoleThug_CheckDropY - pole-thug native switch, every frame of its motion coroutine:
 * 1 once its y reaches script variable 1 (jump off the pole to the floor), 2 once it reaches
 * variable 0 (which then becomes $7FFF, so it fires once), else 0
 * In:  vm = the thug's a4 block (var 0, var 1: y thresholds), self = the thug (y)
 * Out: d0 = 0 / 1 / 2 for Op2_CallNativeRelSwitch (moveq: the whole long)
 * Evidence: move.w $42(a6),d0 / cmp.w $24(a4),d0 / bge -> 1 / cmp.w $22(a4),d0 / bge -> 2 with
 *           move.w #$7FFF,$22(a4); only caller the op $2D30 at $025820; every spawn entry gives
 *           var 0 = $7FFF, so case 2 never fires in the corpus; TAS f21270-f30032 (1626 frames)
 */
uint32_t Enemy_PoleThug_CheckDropY(Vm2Block *vm, const Vm2Obj *self)
{
    int16_t y = self->y;
    if (y >= (int16_t)vm->var[1])   /* bge: signed */
        return 1;
    if (y >= (int16_t)vm->var[0]) {
        vm->var[0] = 0x7FFF;
        return 2;
    }
    return 0;
}
