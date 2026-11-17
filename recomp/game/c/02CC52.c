#include "game.h"

/* the object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02CC5E = 0x02CC5Eu;

/* Enemy_Gunship_SpawnB - wave spawner: the second grey gunship (Scr2_02CC5E, body $2800A) that
 * cycles through 4 player-relative windows ($1680 ticks each: below-right, above-right, above-left,
 * below-left); its attack fires fans of 4 homing missiles (headings $230/$215/$1EB/$1D0) twice,
 * then 4 single missiles heading $200 $780 ticks apart, and loops
 * In:  x/y = where the trigger or wave entry places it
 * Out: script = Scr2_02CC5E, then Obj_SpawnEnemy: Z set and a6 = the new object when it spawned, Z
 *      clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_02CC5E,a2 / jmp Obj_SpawnEnemy; Scr5_021D3A +$490; Op2_SetVector2HereGotoArgs4
 *           windows into $27D34; the fan subroutines $2CD8E-$2CE7C use Spawn_AtOwnOfs with
 *           Enemy_Missile_SpawnGunshipA-D; f65900 shows the grey ship at the right edge; TAS
 *           65685-66611
 */
void Enemy_Gunship_SpawnB(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    *script = Scr2_02CC5E;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
