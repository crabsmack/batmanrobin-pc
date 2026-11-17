#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02D002 = 0x02D002u;

/* Enemy_Missile_SpawnStraightFast - spawns a fast, nearly straight homing missile (Scr2_02D002:
 * cruise $5AA, turn rate $100, the missile body) with heading, straight and homing times from
 * g_VM2Arg0-2
 * In:  d4/d5 = x/y (the launcher's), g_VM2Arg0-2
 * Out: script = Scr2_02D002, then Obj_SpawnEnemyOrQuit's results, then Enemy_Missile_InitFromArgs's
 * Evidence: spawner long(s) at $02CF4C; lea script / jsr Obj_SpawnEnemyOrQuit / jmp
 *           Enemy_Missile_InitFromArgs(pc) as its siblings; raw until crane-crash (2026-10-30): no
 *           corpus run spawns it; 02CFF2.syn checks it
 */
void Enemy_Missile_SpawnStraightFast(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t list = 0, sprite = 0;
    *script = Scr2_02D002;
    Obj_SpawnEnemyOrQuit(d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Enemy_Missile_InitFromArgs);   /* its a3/a4 = the new missile's blocks */
}
