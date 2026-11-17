#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_045400 = 0x045400u;

/* Enemy_JetTrooper_SpawnHoverOnJitter - spawns a jetpack trooper (body $423CE) placed with a wide
 * jitter ($12C, $C8) that hovers in a window on the targeted player (args $50,-$50,$50,-$50) at cruise
 * $1AA and turn rate $555, re-aiming every $3C0 ticks
 * In:  d4/d5 = position
 * Out: script (a2) = Scr2_045400, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_045400,a2 / jmp Obj_SpawnEnemy; the stub shape in the jet trooper spawner series;
 *           Scr2_045400: Enemy_JetTrooper_SpawnHoverOn's script with turn rate $555, accel/cruise $1AA,
 *           JitterPos $12C $C8, timer $3C0; the four ROM longs $000453F4 ($01317E, $0131EC, $046AB6,
 *           $046BAA) are a4-script words (Op4_Spawn / Op4_Fork operand 0004, then the op $53F4), not
 *           references: no caller known; no corpus run reaches it (raw-stubs 2026-10-30); the contract
 *           is Enemy_JetTrooper_SpawnHoverOn's (the same kind of call); 0453F4.syn checks it
 */
void Enemy_JetTrooper_SpawnHoverOnJitter(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_045400;
    dk::jump(at_Obj_SpawnEnemy);
}
