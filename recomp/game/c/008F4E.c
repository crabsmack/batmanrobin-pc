#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* Scr2_002D28: a script that is just the return op ($2D2A): a coroutine slot's idle value */
static constexpr uint32_t Scr2_002D28 = 0x002D28u;

enum {
    INIT_MOTION_CLEAR = 0x40,   /* bytes of the motion block cleared (16 longs) */
    INIT_SPRITE_DEPTH = 1,      /* the sort depth a new object starts at */
};

/* Obj_InitSpawned - initialises a freshly allocated script object: clears its main block's script
 * state and the head of each extra block, gives every coroutine slot the idle script, and sets the
 * position, a serial number, owner id 0 and the scene's sprite attribute.
 * In:  a6 = the object (+8/+$A/+$C = the motion, VM and sprite block handles); d4/d5 = x/y
 * Out: a3/a4/a5 = the blocks (sign-extended handles); a0 = sprite + $2A (where the clear stopped);
 *      d0 = 0, Z set
 * Evidence: movea.w $8(a6),a3 / $A(a6),a4 / $C(a6),a5; clears $18..$31 of the object (6 longs and a
 *           word), $40 bytes of a3, $2A of a4 and a5 (10 longs and a word each); move.l #$2D28 to
 *           $38/$3C/$40/$44(a4); move.w d4,$3E(a6) / d5,$42(a6); $56 = g_ObjSerial, +1; $58 = 0;
 *           move.w #1,$3A(a5); move.w g_SceneSprAttr,$38(a5); moveq #0,d0. Reached by the beq of
 *           Obj_Spawn / Obj_SpawnUnlinked. Prototype: &ccr added (the moveq's flags, its ccout)
 */
void Obj_InitSpawned(uint32_t *d0, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                     uint32_t a6, dk::Flags *ccr)
{
    Vm2Obj *self = ptr<Vm2Obj>(a6);
    *a3 = (uint32_t)(int32_t)self->motion;
    *a4 = (uint32_t)(int32_t)self->vm;
    *a5 = (uint32_t)(int32_t)self->sprite;
    Vm2Motion *mot = ptr<Vm2Motion>(*a3);
    Vm2Block *vm = ptr<Vm2Block>(*a4);
    Vm2Sprite *spr = ptr<Vm2Sprite>(*a5);

    dk::mem_set(&self->population, 0, offsetof(Vm2Obj, vector) - offsetof(Vm2Obj, population));
    dk::mem_set(mot, 0, INIT_MOTION_CLEAR);
    dk::mem_set(vm, 0, offsetof(Vm2Block, resume));
    dk::mem_set(spr, 0, offsetof(Vm2Sprite, draw));
    *a0 = *a5 + (uint32_t)offsetof(Vm2Sprite, draw);

    for (int i = 0; i < 4; i++)
        vm->coroutine[i].v = Scr2_002D28;
    self->x = (int16_t)d4;
    self->y = (int16_t)d5;
    self->serial = g_ObjSerial;
    g_ObjSerial++;
    self->owner_id = 0;
    spr->depth = INIT_SPRITE_DEPTH;
    spr->attr = g_SceneSprAttr;
    *d0 = 0;
    *ccr = dk::Flags::test(0, 4, ccr->x);
}
