#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0316B0 = 0x0316B0u;

/* Enemy_Missile_SpawnAgileSlow - spawns a homing missile with Enemy_Missile_SpawnAgile's tight turn
 * (turn rate $A00, coroutine 0 $28836, body Scr2_0287B0) but a lower cruise $355 (reached over $50),
 * and sets heading/straight/homing time from g_VM2Arg0-2
 * In:  d4/d5 = position (Spawn_AtOwnOfs, offset ($A, 0)); g_VM2Arg0-2 from the launcher's GosubArgs
 * Out: as Enemy_Missile_InitFromArgs; a short pool: Obj_SpawnEnemyOrQuit returns past this function
 * Evidence: lea Scr2_0316B0,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Enemy_Missile_InitFromArgs;
 *           Scr2_0316B0; dc.l of Op2_CallNative Spawn_AtOwnOfs at $031694 in the launch subroutine
 *           $031690, which no Gosub, GosubArgs or ROM long names: no reachable caller is known; no
 *           corpus run reaches it (raw-stubs 2026-10-30); the contract and the C are
 *           Enemy_Missile_SpawnAgile's (the same call); 03169E.syn checks it
 */
void Enemy_Missile_SpawnAgileSlow(uint32_t *d0, uint16_t *d1, uint32_t x, uint32_t y, uint32_t *d7, uint32_t *motion, uint32_t *vm, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read; a6
       (the new object) is this prototype's clobber */
    uint32_t list = 0, sprite = 0, self = a6;
    /* d4/d5 whole: the short-pool path returns from Obj_SpawnEnemyOrQuit past this function */
    EnemyMissileSpawnFn(at_Obj_SpawnEnemyOrQuit, d0, d1, x, y, d7, &list, Scr2_0316B0, motion, vm, &sprite, &self);
    dk::jump(at_Enemy_Missile_InitFromArgs);   /* jmp: its a3/a4 = the new object's blocks */
}
