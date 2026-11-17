#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

enum {
    SPAWN_BOSS_BLOCKS = 4,     /* main block + motion, VM and sprite blocks */
    SPAWN_OWNER_BOSS = 0xB,    /* Vm2Obj.owner_id of the boss class */
};

/* Obj_SpawnBossOrQuit - spawns a 4-block boss-class script object at d4/d5, returning to the
 * caller's caller when the pool is short.
 * In:  a2 = the script; d4/d5 = x/y
 * Out: a6 = the object (a3-a5 its blocks); the pool short: one return address dropped. Z clear
 *      either way: move.w #$B,$58(a6) on success (N V C clear, as Obj_Spawn's moveq #0 left them);
 *      Obj_Spawn's moveq #-1 on failure (the stub keeps its N)
 * Evidence: moveq #4,d7 / lea ($7D2).w,a0 / bsr.w Obj_Spawn / bne.s (addq.l #4,a7 / rts) /
 *           move.l a2,$2A(a4) / move.w #$B,$58(a6) / rts; callers $13F02, $2EA1E, $3DA2E
 */
bool Obj_SpawnBossOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2,
                         uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d7 = SPAWN_BOSS_BLOCKS;
    *a0 = at_VM2_RunObject;
    uint16_t blocks = (uint16_t)*d7;
    Obj_Spawn(d0, d1, d4, d5, &blocks, a0, a3, a4, a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
    if (*d0 != 0) {   /* Obj_Spawn's Z clear: the pool is short */
        dk::drop_return();
        return false;
    }
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_BOSS;
    return false;
}

/* Obj_SpawnBossUnlinked - spawns a 4-block boss-class script object at d4/d5 without linking it on
 * the main object list (its owner runs it: the stage 2 arena boss's turrets).
 * In:  a2 = the script; d4/d5 = x/y
 * Out: d0 = 0, a4 = its VM block, a6 = the object; Z clear when the pool is short
 * Evidence: moveq #4,d7 / lea ($7D2).w,a0 / bsr.w Obj_SpawnUnlinked / bne.s rts / move.l a2,$2A(a4)
 *           / move.w #$B,$58(a6) / moveq #0,d0; 24 callers ($260F2, $26158, $2F038, $3B3B8...)
 */
void Obj_SpawnBossUnlinked(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t a2,
                           uint32_t *a4, uint32_t *a6)
{
    uint32_t a0 = at_VM2_RunObject, a3 = 0, a5 = 0;
    *d7 = SPAWN_BOSS_BLOCKS;
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnUnlinked(d0, d1, d4, d5, &blocks, &a0, &a3, a4, &a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
    if (*d0 != 0)
        return;   /* Obj_SpawnUnlinked's Z clear: the pool is short */
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_BOSS;
    *d0 = 0;      /* moveq #0: Z set, as Obj_SpawnUnlinked left it */
}
