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
    SPAWN_OWNER_BOSS = 0xB,    /* Vm2Obj.owner_id of the boss class ($39AC scales its energy by g_F64C) */
};

/* Obj_SpawnBossN - spawns a d7-block a2-script object at d4/d5, linked on the main object list, in
 * the boss class (owner id $B).
 * In:  d7 = blocks; a2 = the script; d4/d5 = x/y
 * Out: d0 = 0, a6 = the object, a3-a5 its blocks; the pool short: Obj_Spawn's d0 = -1, Z clear
 * Evidence: lea ($7D2).w,a0 / bsr.w Obj_Spawn / bne.s rts / move.l a2,$2A(a4) / move.w #$B,$58(a6)
 *           / moveq #0,d0; caller Boss_JokerBalloon_Spawn (d7 = 7). Prototype: d0, d7, a3 outputs
 *           (measured clobbers) and d7 in/out: Obj_SpawnBoss, which falls into it, hands them back
 */
void Obj_SpawnBossN(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint16_t *d7, uint32_t *a0, uint32_t a2,
                    uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *a0 = at_VM2_RunObject;
    Obj_Spawn(d0, d1, d4, d5, d7, a0, a3, a4, a5, a6);
    if (*d0 != 0)
        return;   /* Obj_Spawn's Z clear: the pool is short */
    ptr<Vm2Block>(*a4)->resume.v = a2;
    ptr<Vm2Obj>(*a6)->owner_id = SPAWN_OWNER_BOSS;
    *d0 = 0;      /* moveq #0: Z set, as Obj_Spawn left it */
}

/* Obj_SpawnBoss - spawns a 4-block boss-class script object at d4/d5 (Obj_SpawnBossN with d7 = 4).
 * In:  a2 = the script; d4/d5 = x/y
 * Out: d0 = 0, a6 = the object (a3-a5 its blocks); Z clear when the pool is short
 * Evidence: moveq #4,d7, falling into Obj_SpawnBossN; 16 callers, mostly once per stage ($1805A on
 *           TAS f4518 before the stage 1 boss)
 */
void Obj_SpawnBoss(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2,
                   uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *d7 = SPAWN_BOSS_BLOCKS;   /* moveq: the whole register */
    uint16_t blocks = (uint16_t)*d7;
    Obj_SpawnBossN(d0, d1, d4, d5, &blocks, a0, a2, a3, a4, a5, a6);
    *d7 = (*d7 & 0xFFFF0000u) | blocks;
}
