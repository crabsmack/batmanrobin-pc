#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

enum {
    SPAWN_ENEMY_BLOCKS = 4,     /* main block + motion, VM and sprite blocks */
    SPAWN_OWNER_ENEMY = 0xA,    /* Vm2Obj.owner_id of the enemies (copied into their boxes) */
};

/* Obj_SpawnEnemyN - spawns a d7-block a2-script object (VM2_RunObject) at d4/d5, linked on the
 * main object list, as an enemy (owner id $A) running script a2.
 * In:  d7 = blocks; a2 = the script; d4/d5 = x/y
 * Out: d0 = 0, a6 = the object, a3-a5 = its blocks; the pool short: Obj_Spawn's d0 = -1, Z clear
 * Evidence: lea ($7D2).w,a0 / bsr.w Obj_Spawn / bne.s rts / move.l a2,$2A(a4) / move.w #$A,$58(a6)
 *           / moveq #0,d0; callers $3904E, $39176, $391E4, $3925C (d7 = 5). Prototype: d0 an output
 *           (measured a clobber): Obj_SpawnEnemy, which falls into it, hands it back
 */
void Obj_SpawnEnemyN(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t a2,
                     uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = at_VM2_RunObject;
    Obj_Spawn(d0, d1, d4, d5, d7, a0, a3, a4, a5, a6);
    if (*d0 != 0)
        return;   /* Obj_Spawn's Z clear: the pool is short */
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_ENEMY;
    *d0 = 0;      /* moveq #0: Z set, as Obj_Spawn left it */
}

/* Obj_SpawnEnemy - spawns a 4-block enemy script object at d4/d5 (Obj_SpawnEnemyN with d7 = 4).
 * In:  a2 = the a2-VM script; d4/d5 = x/y
 * Out: d0 = 0 and a6 = the object (a3-a5 its blocks); Z clear (d0 = -1) when the pool is short
 * Evidence: moveq #4,d7, falling into Obj_SpawnEnemyN; 199 callers `lea Scr2_x,a2 / jmp
 *           Obj_SpawnEnemy` ($11B32, $133DE...)
 */
void Obj_SpawnEnemy(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2,
                    uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d7 = SPAWN_ENEMY_BLOCKS;   /* moveq: the whole register */
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnEnemyN(d0, d1, d4, d5, &blocks, a0, a2, a3, a4, a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
}
