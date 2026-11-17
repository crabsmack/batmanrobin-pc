#include "game.h"

/* the pickup's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_013A06 = 0x013A06u;

/* Item_SpawnWeapon0FromHit - spawn action 7: the weapon type 0 pickup (value 1) knocked out of a
 * struck object
 * In:  x/y = where the pickup appears (SpawnAction_Run passes the dying object's position)
 * Out: Obj_SpawnEnemyOrQuit's d0/d1/d7, motion = the new pickup's motion block, then
 *      Item_LaunchFromHit (x velocity, d0 = 0); a short pool: Obj_SpawnEnemyOrQuit returns
 *      past this to SpawnAction_Run with d0 nonzero
 * Evidence: lea Scr2_013A06,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Item_LaunchFromHit; Tbl_012EF0
 *           entry 7; Scr2_013A06 = coroutine $13378, table $13C62 (fixed value 1), then
 *           Item_LaunchFromHit; TAS frames 82651-82821
 */
void Item_SpawnWeapon0FromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    Obj_SpawnEnemyOrQuit(result, d1, x, y, d7, &list, Scr2_013A06, motion, &a4, &sprite, &a6);
    dk::jump(at_Item_LaunchFromHit);   /* its a3: the new pickup's motion block */
}
