#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_04446C = 0x04446Cu;

/* Enemy_HoverPod_Spawn5E - spawns a Freeze-fight hover pod with the drop table $438DC on course 5
 * ($4479A: the course-4 waypoints from the second) with attack E ($44AAA: rounds of five shots $40
 * ticks apart), x cruise $1AA, y $155
 * In:  d4/d5 = position (Spawn_AtScreenCentre, the Freeze fight script's Op2_CallNative $008D12)
 * Out: script (a2) = Scr2_04446C, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_04446C,a2 / jmp Obj_SpawnEnemy; Scr2_04446C: gosub $438DC, energy $200/$240,
 *           coroutine 0 $4479A, coroutine 1 $44AAA, JitterPos $12C $28, MoveY -$96, Goto $42152; dc.l
 *           of Op2_CallNative Spawn_AtScreenCentre at $0470EE, $04712C in the Freeze fight scripts; no
 *           corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_GasOrb_SpawnBossPairJets's (the same kind of call); 044460.syn checks it
 */
void Enemy_HoverPod_Spawn5E(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_04446C;
    dk::jump(at_Obj_SpawnEnemy);
}
