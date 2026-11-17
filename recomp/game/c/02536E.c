#include "game.h"

/* a 16.16 position from its integer and fraction words */
static int32_t BarrelThug_Fixed(int16_t whole, uint16_t frac)
{
    return (int32_t)(((uint32_t)(uint16_t)whole << 16) | frac);
}

/* Enemy_BarrelThug_ThrowBarrel - barrel thug native with two inline offset words: spawns a barrel
 * at the thug's position + (dx, dy), rolling toward the targeted player, and hands it the thug's
 * drop (the thug then drops nothing itself)
 * In:  script = the inline dx, dy words (a2); vm = the thug's a4 block (tracked = the targeted
 *      player record, 0 none); self = the thug (a6); a1/a3/a5 only for the saved-register image
 * Out: d0 = 0 thrown (the barrel's x velocity and target speed = (player x - x) / 128 * 1.25,
 *      16.16, or 0 with no target; its spawn action/argument = the thug's, the thug's action
 *      cleared), d0 = -1 when the pool is full; d1 = speed / 4 when a player is tracked; d4/d5 =
 *      the barrel's x/y; d7 as Obj_SpawnEnemy leaves it; script past the two words
 * Evidence: move.w $10(a4),d0 / ext.l / beq / movea.w d0,a0 / move.l $2E(a0),d0 / sub.l $3E(a6) /
 *           asr.l #7 / d1 = d0 asr 2 / add.l; movem.l a1-a6 + d0 pushed around jsr
 *           Enemy_Barrel_Spawn; called from the throw animation at $025098 (dy -$26: overhead) and
 *           the death path $024DAE (dy -$1A: drops it); TAS f26895-f31863 (32). The prototype
 *           gains a1/a5 inputs (only pushed: the stack image the asm leaves)
 */
void Enemy_BarrelThug_ThrowBarrel(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t a1,
                                  const s16 **script, uint32_t a3, const Vm2Block *vm, uint32_t a5, Vm2Obj *self)
{
    int16_t h = vm->tracked;
    int32_t speed = h;   /* ext.l: 0 when no player is tracked */
    if (h != 0) {
        const PlayerRec *rec = dk::handle<const PlayerRec>(h);
        speed = BarrelThug_Fixed(rec->x, rec->x_frac) - BarrelThug_Fixed(self->x, self->x_frac);
        speed >>= 7;                    /* asr.l: rounds toward minus infinity */
        *d1 = (uint32_t)(speed >> 2);
        speed += speed >> 2;            /* times 1.25 */
    }
    *d0 = (uint32_t)speed;
    *d4 = (uint16_t)(self->x + *(*script)++);
    *d5 = (uint16_t)(self->y + *(*script)++);

    /* movem.l a1-a6,-(a7) / move.l d0,-(a7): the same stack image the asm leaves */
    uint32_t a2 = dk::addr(*script), a4 = dk::addr(vm), a6 = dk::addr(self);
    dk::push_long(a6);
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);
    dk::push_long(*d0);

    uint32_t result = 0, barrel_script = 0, barrel_motion = a3, barrel = a6;
    uint16_t d1w = (uint16_t)*d1;
    Enemy_Barrel_Spawn(&result, &d1w, *d4, *d5, d7, &barrel_script, &barrel_motion, a4, &barrel);
    *d1 = (*d1 & 0xFFFF0000u) | d1w;

    *d0 = dk::pop_long();
    for (int i = 0; i < 6; i++)
        (void)dk::pop_long();   /* movem.l (a7)+,a1-a6: the thug's registers, which C kept */
    if (result != 0) {          /* bne: the pool is full */
        *d0 = 0xFFFFFFFFu;
        return;
    }
    PTR(Vm2Motion) mot;         /* the barrel's motion block and object, from the spawn */
    mot.v = barrel_motion;
    mot->target_x = (int32_t)*d0;
    mot->vel_x = (int32_t)*d0;
    PTR(Vm2Obj) obj;
    obj.v = barrel;
    obj->spawn_action = self->spawn_action;
    obj->spawn_arg = self->spawn_arg;
    self->spawn_action = 0;
    *d0 = 0;
}
