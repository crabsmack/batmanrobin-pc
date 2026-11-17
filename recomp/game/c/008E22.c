#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

enum {
    SPAWN_ENEMY_BLOCKS = 4,     /* main block + motion, VM and sprite blocks (as Obj_SpawnEnemy) */
    SPAWN_OWNER_ENEMY = 0xA,    /* Vm2Obj.owner_id of the enemies */
};

/* Obj_SpawnEnemyAfter - Obj_SpawnEnemy with the object linked right after the running object
 * (Obj_SpawnAfter): a 4-block script object (enemy, owner $A) at d4/d5 running script a2
 * In:  a2 = the a2-VM script, d4/d5 = x/y, a6 = the running object
 * Out: d0 = 0 and a6 = the object (its VM block's resume = a2, owner $A); Obj_SpawnAfter's d0 (nonzero,
 *      Z clear) when the pool is short; d7 = 4, a0 = VM2_RunObject
 * Evidence: moveq #4,d7 / lea ($7D2).w,a0 / bsr.w Obj_SpawnAfter / bne.s / move.l a2,$2A(a4) /
 *           move.w #$A,$58(a6) / moveq #0,d0 / rts, Obj_SpawnEnemy's shape; jmp target of the 12 Hatter
 *           road piece spawners ($03FB68-$03FD7E); no corpus run reaches it (raw-stubs 2026-10-30); the
 *           contract is Obj_SpawnEnemy's; 008E22.syn checks it
 */
void Obj_SpawnEnemyAfter(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                         uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d7 = SPAWN_ENEMY_BLOCKS;   /* moveq: the whole register */
    *a0 = at_VM2_RunObject;
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnAfter(d0, d1, d4, d5, &blocks, a0, a3, a4, a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
    if (*d0 != 0)
        return;   /* Obj_SpawnAfter's Z clear: the pool is short */
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_ENEMY;
    *d0 = 0;
}
