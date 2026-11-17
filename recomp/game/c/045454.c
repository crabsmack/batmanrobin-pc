#include "game.h"

/* the spawned script object's a2 script (ROM data right after this spawner's code) */
static constexpr uint32_t Scr2_045460 = 0x045460u;

/* Enemy_JetTrooper_SpawnHoverOn - spawns a jetpack trooper of the stage 4 tube section that hovers in a
 * window on the targeted player (args $50,-$50,$50,-$50), turn rate 0 then
 * $500, slow cruise $155
 * In:  x/y (d4/d5) = the spawn position (wave entry: x $168 right edge, y $70)
 * Out: script (a2) = Scr2_045460, then Obj_SpawnEnemy's results (d0 = 0 and a6 = the new object, nonzero d0
 *      when the pool is short; its condition codes)
 * Evidence: lea Scr2_045460,a2 / jmp Obj_SpawnEnemy; Scr2_045460: speed $100, population +1, score $1F4, drop
 *           init $439C6, energy $1B0/$1E0 (1P/2P), coroutine 0 $45486 (Op2_GosubArgs into
 *           Op2_SetPlayerWindowFromArgs $424B0, waits $780), cruise $155, body $423CE; wave entries $41CAC,
 *           $41D04 (tube wave Scr2_0417CC) and $41710; RAM f125700 an object of this body at (193, 29) = the
 *           white armoured trooper with a jetpack and a gun (shot w29_c125700); scenario-level-skip f13292,
 *           fuzz runs
 */
void Enemy_JetTrooper_SpawnHoverOn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3,
                                   uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045460;
    dk::jump(at_Obj_SpawnEnemy);
}
