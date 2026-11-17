#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0455FE = 0x0455FEu;

/* Enemy_JetTrooper_SpawnHoverOnFast - spawns a tube-section jetpack trooper that hovers in a window on
 * the targeted player (args $50,-$50,$50,-$50) as Enemy_JetTrooper_SpawnHoverOn but at cruise $1AA,
 * re-aiming after $3C0 ticks
 * In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
 * Out: script (a2) = Scr2_0455FE, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_0455FE,a2 / jmp Obj_SpawnEnemy; Scr2_0455FE: Enemy_JetTrooper_SpawnHoverOn's
 *           script (gosub $439C6, turn rate 0 then $500, body $423CE) with accel/cruise $1AA and timer
 *           $3C0; the tube wave's spawn entries at $041CD4 and $041CDC (x -$28 / $168, y $E070) name
 *           it; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_JetTrooper_SpawnHoverOn's (the same kind of call); 0455F2.syn checks it
 */
void Enemy_JetTrooper_SpawnHoverOnFast(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_0455FE;
    dk::jump(at_Obj_SpawnEnemy);
}
