#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02C6F2 = 0x02C6F2u;

/* Enemy_Gunship_SpawnA - wave spawner: the grey gunship (Scr2_02C6F2, body $2800A: energy $A00,
 * contact attack box with knockback) that holds 240-260 px right of the targeted player for 2 x
 * $2580 ticks, closes in onto it (+-$14) for $3840, repeats, then dashes 300 px left at cruise $D55
 * and starts over; launches homing missiles: 2 rounds of 10 fast single ones (subroutines
 * $2CF1A-$2CFAE), then 4 fans of 4 (Gunship B's $2CD8E-$2CDE8), then 5 rounds of 4 single ones
 * heading $200
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02C6F2, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02C6F2,a2 / jmp Obj_SpawnEnemy; Tbl_023A84 +$B0 (drop action $2D) and
 *           Scr5_023B38 +$14; Op2_GosubArgs entry $2D04 (3 args: homing time, straight time,
 *           heading) into subroutines that call Spawn_AtOwnOfs with Enemy_Missile_SpawnGunship*
 *           unless a player box is within $1E px; shot at f52500: a large grey ship firing at the
 *           right edge; TAS 52346-57737
 */
void Enemy_Gunship_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02C6F2;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
