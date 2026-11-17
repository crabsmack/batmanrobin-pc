#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A4CE = 0x04A4CEu;

/* StageCard_SpawnTitle4 - spawns the LEVEL FOUR title object (sprite text sliding in from the left)
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: Obj_SpawnEnemy's results (d0 = 0 / -1; the object's registers are this prototype's clobbers)
 * Evidence: lea Scr2_04A4CE,a2 / jmp Obj_SpawnEnemy; Scr2_04A4CE Text_DrawAtObject string $140E4 'LEVEL
 *           FOUR'; entered by the stage 4 director
 */
void StageCard_SpawnTitle4(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6)
{
    /* a jmp, called: a2 (the script) is not this prototype's to hand on, and the spawned
       object's a3-a6 are its clobbers; Obj_SpawnEnemy plays no stack tricks */
    uint32_t a0 = 0, a3 = 0, vm = a4, a5 = 0, self = a6;
    Obj_SpawnEnemy(d0, d1, x, y, d7, &a0, Scr2_04A4CE, &a3, &vm, &a5, &self);
}
