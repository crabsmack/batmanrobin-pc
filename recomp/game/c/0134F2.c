#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013502 = 0x013502u;

/* Item_SpawnSmallHealthFromHit - spawn action 34: the small health pickup (+$10 health, sound $12;
 * body $1351E) knocked out of a struck object: pops up (motion coroutine $13378) with half the
 * hit's knockback as x velocity
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: Obj_SpawnEnemyOrQuit's d0/d1/d7, motion = the new pickup's motion block, then
 *      Item_LaunchFromHit (x velocity, d0 = 0); a short pool: Obj_SpawnEnemyOrQuit returns past
 *      this to SpawnAction_Run with d0 nonzero
 * Evidence: lea Scr2_013502,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Item_LaunchFromHit; Tbl_012EF0
 *           entry 34; raw until crane-crash (2026-10-30): no corpus run spawns it; 0134F2.syn
 *           checks it
 */
void Item_SpawnSmallHealthFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    Obj_SpawnEnemyOrQuit(result, d1, x, y, d7, &list, Scr2_013502, motion, &a4, &sprite, &a6);
    dk::jump(at_Item_LaunchFromHit);   /* its a3: the new pickup's motion block */
}
