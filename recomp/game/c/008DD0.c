#include "game.h"

/* Obj_SpawnEnemyOrQuit - spawns a 4-block enemy script object like Obj_SpawnEnemy, but returns to
 * the caller's caller when the pool is short (Obj_SpawnEnemyNOrQuit with d7 = 4).
 * In:  a2 = the script; d4/d5 = x/y
 * Out: d7 = 4; then Obj_SpawnEnemyNOrQuit's: a6 = the object, or one return address dropped
 * Evidence: moveq #4,d7, falling into Obj_SpawnEnemyNOrQuit (the next file); 62 callers ($133F2,
 *           $1FFA6, $20082, $29DCC...)
 */
void Obj_SpawnEnemyOrQuit(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0, uint32_t a2,
                          uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d0, (void)d1, (void)d4, (void)d5, (void)a0, (void)a2, (void)a3, (void)a4, (void)a5, (void)a6;
    *d7 = 4;
    dk::jump(at_Obj_SpawnEnemyNOrQuit);
}
