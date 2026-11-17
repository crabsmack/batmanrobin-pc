#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013402 = 0x013402u;

/* Item_SpawnBigHealthFromHit - spawn action 31: the large health pickup (+$80 health, sound $41)
 * knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback
 * as x velocity
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: Obj_SpawnEnemyOrQuit's d0/d1/d7, motion = the new pickup's motion block, then
 *      Item_LaunchFromHit (x velocity, d0 = 0); a short pool: Obj_SpawnEnemyOrQuit returns
 *      past this to SpawnAction_Run with d0 nonzero
 * Evidence: lea Scr2_013402,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Item_LaunchFromHit; Tbl_012EF0
 *           entry 31; Scr2_013402: coroutine 1 = $13378 (Op2_SetCruiseY $640, Op2_JumpAnim,
 *           bounce); then Item_LaunchFromHit; 7 TAS frames (7258-80494)
 */
void Item_SpawnBigHealthFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    Obj_SpawnEnemyOrQuit(result, d1, x, y, d7, &list, Scr2_013402, motion, &a4, &sprite, &a6);
    dk::jump(at_Item_LaunchFromHit);   /* its a3: the new pickup's motion block */
}
