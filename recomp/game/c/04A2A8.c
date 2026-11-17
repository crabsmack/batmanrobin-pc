#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A2B4 = 0x04A2B4u;

/* StageCard_SpawnSubtitle1 - spawns the HAPPY BIRTHDAY TO ME! subtitle object: sprite text that enters
 * from the right (x 320, y 210) and arcs left to a stop
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: Obj_SpawnEnemy's results (d0 = 0 / -1; the object's registers are this prototype's clobbers)
 * Evidence: lea Scr2_04A2B4,a2 / jmp Obj_SpawnEnemy; Scr2_04A2B4: SetPos ($140, $D2), ArcX ($13B,
 *           $3C0), Text_DrawAtObject string $1409C; f832; png f840 shows HAPPY at the right edge
 */
void StageCard_SpawnSubtitle1(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6)
{
    /* a jmp, called: a2 (the script) is not this prototype's to hand on, and the spawned
       object's a3-a6 are its clobbers; Obj_SpawnEnemy plays no stack tricks */
    uint32_t a0 = 0, a3 = 0, vm = a4, a5 = 0, self = a6;
    Obj_SpawnEnemy(d0, d1, x, y, d7, &a0, Scr2_04A2B4, &a3, &vm, &a5, &self);
}
