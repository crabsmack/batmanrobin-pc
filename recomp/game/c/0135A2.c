#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0135B2 = 0x0135B2u;

/* Item_SpawnExtraLifeFromHit - spawn action 28: the extra-life pickup (lives +1, sound $42) knocked
 * out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x
 * velocity
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: Obj_SpawnEnemyOrQuit's d0/d1/d7, motion = the new pickup's motion block, then
 *      Item_LaunchFromHit (x velocity, d0 = 0); a short pool: Obj_SpawnEnemyOrQuit returns
 *      past this to SpawnAction_Run with d0 nonzero
 * Evidence: lea Scr2_0135B2,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Item_LaunchFromHit; Tbl_012EF0
 *           entry 28; Scr2_0135B2: coroutine 1 = $13378 (Op2_SetCruiseY $640, Op2_JumpAnim,
 *           bounce); then Item_LaunchFromHit; 4 TAS frames (7200-115744)
 */
void Item_SpawnExtraLifeFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    Obj_SpawnEnemyOrQuit(result, d1, x, y, d7, &list, Scr2_0135B2, motion, &a4, &sprite, &a6);
    dk::jump(at_Item_LaunchFromHit);   /* its a3: the new pickup's motion block */
}
