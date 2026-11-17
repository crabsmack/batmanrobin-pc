#include "game.h"

/* Stage1_TrashCanHasDrop - trash-can native switch: 1 when the can carries a spawn action (a drop),
 * else 0, so its death script spawns the drop effect first
 * In:  self = the trash can (a6): spawn_action
 * Out: d0 = 1 with a drop; without one d0's low word = 0 (move.b / ext.w: the high word stays)
 * Evidence: move.b $19(a6),d0 / ext.w d0 / beq / moveq #1,d0 / rts; Op2_CallNativeRelSwitch at $020540
 *           in Scr2_020504 (case 1 = Op2_FxSpawnHere $02059C, then Op2_AwardScoreToHitter); TAS
 *           f603-f4385, the two-player scenario f1722
 */
void Stage1_TrashCanHasDrop(uint32_t *d0, const Vm2Obj *self)
{
    if (self->spawn_action != 0)
        *d0 = 1;
    else
        *d0 &= 0xFFFF0000u;           /* ext.w of 0: the low word only */
}
