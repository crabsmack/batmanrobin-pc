#include "game.h"

/* Enemy_BombDropper_DropBomb - native in the bomb dropper's drop animation: spawns a bouncing bomb
 * at its position plus the inline offset, thrown at the tracked player: x speed = (player x - own
 * x) / 128 * 1.25 into the bomb's x velocity and target speed, and the same tracked record
 * In:  script = the native's arguments (a2: words dx, dy); motion/vm/self = the dropper's blocks
 *      (a3/a4/a6: the tracked player record at vm +$10)
 * Out: d0 = 0, or -1 when the pool is full; script past the two words; d4/d5 = the bomb's spawn
 *      position (low words; the high words kept); d1/d7 as Obj_SpawnBoss left them; the new bomb's
 *      motion x velocity and target speed = the x speed, its tracked record = the dropper's
 * Evidence: move.w $10(a4),d0 / movea.w d0,a0 / beq / move.l $2E(a0),d0 / sub.l $3E(a6),d0 /
 *           asr.l #7,d0 / move.l d0,d1 / asr.l #2,d1 / add.l d1,d0 / add.w (a2)+,d4 / add.w (a2)+,d5
 *           / jsr Enemy_BouncingBomb_Spawn / bne / move.w a0,$10(a4) / move.l d0,$24(a3) /
 *           move.l d0,$18(a3) / moveq #0,d0; `2CB2 0050 0000 0014` at $360F4; TAS f79427-f86692 (25
 *           drops). The movem pairs only keep a1-a6 and the speed over the call: the C keeps them
 *           in locals (the bridge restores the registers that are no outputs)
 */
void Enemy_BombDropper_DropBomb(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7,
                                const u16 **script, Vm2Motion *motion, Vm2Block *vm, const Vm2Obj *self)
{
    int16_t tracked = vm->tracked;
    uint32_t speed = (*d0 & 0xFFFF0000u) | (uint16_t)tracked;   /* move.w: the handle in d0's low word */
    if (tracked != 0) {
        const PlayerRec *rec = dk::handle<const PlayerRec>(tracked);
        uint32_t rec_x = (uint32_t)(uint16_t)rec->x << 16 | rec->x_frac;
        uint32_t own_x = (uint32_t)(uint16_t)self->x << 16 | self->x_frac;
        int32_t dx = (int32_t)(rec_x - own_x) >> 7;
        *d1 = (uint32_t)(dx >> 2);
        speed = (uint32_t)dx + *d1;
    }
    *d4 = (uint16_t)(self->x + (int16_t)*(*script)++);
    *d5 = (uint16_t)(self->y + (int16_t)*(*script)++);
    uint32_t result = 0, bomb_script = 0, bomb_motion = 0, bomb_vm = 0;
    uint16_t d1_low = (uint16_t)*d1;   /* Obj_Spawn writes only d1's low word */
    Enemy_BouncingBomb_Spawn(&result, &d1_low, *d4, *d5, d7, &bomb_script, &bomb_motion, &bomb_vm, dk::addr(self));
    *d1 = (*d1 & 0xFFFF0000u) | d1_low;
    if (result != 0) {   /* bne: Obj_Spawn's -1 (Z clear) exactly when the pool was short */
        *d0 = 0xFFFFFFFFu;
        return;
    }
    PTR(Vm2Motion) bm;
    bm.v = bomb_motion;
    PTR(Vm2Block) bv;
    bv.v = bomb_vm;
    bv->tracked = tracked;   /* move.w a0: the handle */
    bm->target_x = (int32_t)speed;
    bm->vel_x = (int32_t)speed;
    *d0 = 0;
}
