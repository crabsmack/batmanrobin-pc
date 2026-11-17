#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F73A = 0x02F73Au;

/* Flight_SpawnSearchlight - spawns a searchlight on the airship hull: anim $2F7D8 sweeps a lamp's
 * beam through frames $4312-$433A with offset and flip changes, body $2F746
 * In:  x/y = the trigger entry's map position (Flight_AirshipRunTriggers then moves it to world space)
 * Out: script = Scr2_02F73A, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02F73A,a2 / jmp Obj_SpawnEnemy; 6 trigger entries in Tbl_0307C2 at map y $F0;
 *           frames rendered: a lamp with a red-yellow beam turning from up to sideways; TAS
 *           67496-75351 (6). The prototype gains &a2 (measured a clobber: no caller reads it) so
 *           the jump hands the script on
 */
void Flight_SpawnSearchlight(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_02F73A;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
