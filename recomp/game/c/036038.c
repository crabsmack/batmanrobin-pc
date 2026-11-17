#include "game.h"

/* the dropper's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_036050 = 0x036050u;

/* Enemy_BombDropper_Spawn - trigger callback of the bomb dropper (Scr2_036050, energy $40, score
 * $C8): a box hanging on the overhead pipe; once the camera brings it to the entry's screen x it
 * waits the entry's delay, then drops bouncing bombs at the targeted player
 * In:  x/y = trigger position (d4/d5); entry = the trigger entry's extra words (a1): the target
 *      screen x, the delay
 * Out: d0 = 0 (Z set); entry past the two words; the new dropper's motion block point x = the
 *      screen x, its timer 0 = the delay; a4/a5/a6/a0/d1 as Obj_SpawnEnemyOrQuit left them. A
 *      short pool: Obj_SpawnEnemyOrQuit returns past this to the trigger runner
 * Evidence: lea Scr2_036050,a2 / jsr Obj_SpawnEnemyOrQuit / move.w (a1)+,$14(a3) /
 *           move.w (a1)+,$1A(a4) / moveq #0,d0 / rts; dc.l entries in the trigger table Tbl_033CBE
 *           ($033D48-$033E7A); TAS f79360-f86599 (25 spawns). d0 an output (tst(d0.l), measured
 *           as a clobber): the ccout Z the trigger runner reads is the final moveq's, which the C
 *           gives as d0's test (Obj_SpawnEnemyOrQuit's own last move.w leaves Z clear)
 */
uint32_t Enemy_BombDropper_Spawn(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a0, const u16 **entry, uint32_t a3,
                                 uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = 0, d7 = 0;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, &d7, a0, Scr2_036050, &a3, a4, a5, a6);
    PTR(Vm2Motion) motion;
    motion.v = a3;
    PTR(Vm2Block) vm;
    vm.v = *a4;
    motion->point_x = (int16_t)*(*entry)++;
    vm->timer[0] = *(*entry)++;
    return 0;
}
