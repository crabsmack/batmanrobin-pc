#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_030FF4 = 0x030FF4u;

/* Enemy_HullTurret_SpawnAiming - spawns an airship hull gun turret that keeps (or picks) a target,
 * aims and turns toward it (coroutine 0 $2F9EC) and fires bursts of 5 aimed shots (Fx_SpawnAimedN,
 * a4 script $3142A) at random intervals, then swaps target and pauses; start delay timer 0 = $1E0
 * In:  x/y = the trigger entry's map position (Flight_AirshipRunTriggers moves it to world space)
 * Out: script = Scr2_030FF4, then Obj_SpawnEnemy: Z set and a6 = the new turret when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_030FF4,a2 / jmp Obj_SpawnEnemy; 10 trigger entries in Tbl_0307C2 (some with
 *           drop bytes $11/$17); body $2F98C: energy $40, score $64, map-anchored, 16 barrel
 *           directions by heading (frames from $3E1C); TAS 67688-75161 (7 spawns). The prototype
 *           gains &a2 (measured a clobber: the only caller, Trigger_Fire's callback jsr, saves and
 *           restores a2 around it) so the jump hands the script on
 */
void Enemy_HullTurret_SpawnAiming(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_030FF4;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
