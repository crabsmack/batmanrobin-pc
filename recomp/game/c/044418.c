#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_044424 = 0x044424u;

/* Enemy_HoverPod_Spawn4D - spawns a Freeze-fight hover pod with the drop table $438DC on course 4
 * ($44778: waypoint gosubs $45352, $4535E, $45342, $7BC0 ticks each) with attack D ($44962: turn rate
 * $200, shots $A0 ticks apart), energy $200/$240, x/y cruise $155
 * In:  d4/d5 = position (Spawn_AtScreenCentre, the Freeze fight script's Op2_CallNative $008D12)
 * Out: script (a2) = Scr2_044424, then Obj_SpawnEnemy's results
 * Evidence: lea Scr2_044424,a2 / jmp Obj_SpawnEnemy; Scr2_044424: gosub $438DC (drop:
 *           Op2_BranchWeaponLvlMaxGE, SwitchRandom, SetSpawnAction), coroutine 0 $44778, coroutine 1
 *           $44962, JitterPos $12C $28, MoveY -$96, Goto the hover pod body $42152; dc.l of
 *           Op2_CallNative Spawn_AtScreenCentre at $046FB6, $047046, $047056, $0470E4, $047122 in the
 *           Freeze fight scripts; no corpus run reaches it (raw-stubs 2026-10-30); the contract is
 *           Enemy_GasOrb_SpawnBossPairJets's (the same kind of call); 044418.syn checks it
 */
void Enemy_HoverPod_Spawn4D(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t a4, uint32_t a6)
{
    *script = Scr2_044424;
    dk::jump(at_Obj_SpawnEnemy);
}
