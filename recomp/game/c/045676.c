#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045682 = 0x045682u;

/* Enemy_JetTrooper_SpawnAbove - spawns a tube-section jetpack trooper (random speed) that alternates between
 * windows above-right and above-left of the targeted player (y words -$5A/-$64)
 * In:  x/y (d4/d5) = the spawn position (wave entry, x $168 or -$28)
 * Out: script (a2) = Scr2_045682, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045682,a2 / jmp Obj_SpawnEnemy; Scr2_045682: coroutine 0 $456AA: Op2_GosubArgs (-$5A,
 *           -$64, $A0, $50) / (-$5A, -$64, -$50, -$A0), turn rate $300; wave entries $41CE4/$41CEC; RAM
 *           f125220-f125880 slot 0 $456C2-$456E2 at y 29-65 (high on the screen); TAS 125212-128188 (4
 *           spawns)
 */
void Enemy_JetTrooper_SpawnAbove(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                 uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045682;
    dk::jump(at_Obj_SpawnEnemy);
}
