#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_02F722 = 0x02F722u;

/* Flight_SpawnGreenNavLightFlip - spawns the green navigation light H-flipped (Op2_ModSprAttr
 * $9008, the green anim $2F7BA, body $2F746)
 * In:  d4/d5 = x/y (the airship path entry's)
 * Out: script = Scr2_02F722, then as Obj_SpawnEnemy (d0 = 0 and a6 = the new object; nonzero d0
 *      when the pool is short)
 * Evidence: spawner long(s) at $0FDA7A; lea script / jmp Obj_SpawnEnemy as
 *           Flight_SpawnRedNavLightFlip; raw until crane-crash (2026-10-30): no corpus run spawns
 *           it; 02F716.syn checks it
 */
void Flight_SpawnGreenNavLightFlip(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, uint32_t *script, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_02F722;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
