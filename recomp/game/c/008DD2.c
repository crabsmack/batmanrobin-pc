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

/* The ...OrQuit spawners end with move.w #$A,$58(a6) on success: Z clear, N V C clear, as Obj_Spawn
 * leaves them after its moveq #0 (N V C clear). On failure the flags are Obj_Spawn's (moveq #-1:
 * Z clear, N set). So each returns Z clear (->Z) and the stub's flags give the rest. */

/* Obj_SpawnEnemyNOrQuit - spawns a d7-block enemy script object at d4/d5 like Obj_SpawnEnemyN, but
 * returns to the caller's caller when the pool is short.
 * In:  d7 = blocks; a2 = the script; d4/d5 = x/y
 * Out: a6 = the object (a3-a5 its blocks), its script and owner id $A set; the pool short: one
 *      return address dropped (the caller's setup code is skipped); Z clear either way
 * Evidence: lea ($7D2).w,a0 / bsr.w Obj_Spawn / bne.s (addq.l #4,a7 / rts) / move.l a2,$2A(a4) /
 *           move.w #$A,$58(a6) / rts; callers $2658E, $2785E, $278B2, $278CE, Obj_SpawnEnemyOrQuit
 */
bool Obj_SpawnEnemyNOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0,
                           uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = at_VM2_RunObject;
    Obj_Spawn(d0, d1, d4, d5, d7, a0, a3, a4, a5, a6);
    if (*d0 != 0) {   /* Obj_Spawn's Z clear: the pool is short */
        dk::drop_return();
        return false;
    }
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_ENEMY;
    return false;     /* move.w #$A: Z clear */
}

/* Obj_SpawnEnemyUnlinked - spawns a 4-block enemy script object at d4/d5 without linking it on the
 * main object list (Obj_SpawnUnlinked: its owner runs it).
 * In:  a2 = the script; d4/d5 = x/y
 * Out: d7 = Obj_SpawnUnlinked's; a6 = the object; Z set on success (moveq #0,d0), clear when the
 *      pool is short
 * Evidence: moveq #4,d7 / lea ($7D2).w,a0 / bsr.w Obj_SpawnUnlinked / bne.s rts / move.l a2,$2A(a4)
 *           / move.w #$A,$58(a6) / moveq #0,d0; callers $327FE, $32AAA, $32AC2, $32D4E
 */
void Obj_SpawnEnemyUnlinked(uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a2, uint32_t a4, uint32_t *a6)
{
    uint32_t d0 = 0, a0 = at_VM2_RunObject, a3 = 0, a5 = 0;
    uint16_t d1 = 0;
    *d7 = SPAWN_ENEMY_BLOCKS;
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnUnlinked(&d0, &d1, d4, d5, &blocks, &a0, &a3, &a4, &a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
    if (d0 != 0)
        return;   /* Obj_SpawnUnlinked's Z clear: the pool is short */
    ptr<Vm2Block>(a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_ENEMY;
    /* moveq #0,d0: Z set, as Obj_SpawnUnlinked left it (d0 is a clobber) */
}

/* Obj_SpawnEnemyUnlinkedOrQuit - Obj_SpawnEnemyUnlinked that returns to the caller's caller when
 * the pool is short.
 * In:  a2 = the script; d4/d5 = x/y
 * Out: a6 = the object (a3-a5 its blocks); the pool short: one return address dropped; Z clear
 * Evidence: moveq #4,d7 / lea ($7D2).w,a0 / bsr.w Obj_SpawnUnlinked / bne.s (addq.l #4,a7 / rts) /
 *           move.l a2,$2A(a4) / move.w #$A,$58(a6) / rts; caller $3DB50
 */
bool Obj_SpawnEnemyUnlinkedOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                                  uint32_t a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d7 = SPAWN_ENEMY_BLOCKS;
    *a0 = at_VM2_RunObject;
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnUnlinked(d0, d1, d4, d5, &blocks, a0, a3, a4, a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
    if (*d0 != 0) {
        dk::drop_return();
        return false;
    }
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_ENEMY;
    return false;
}
