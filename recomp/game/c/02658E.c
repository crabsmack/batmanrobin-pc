#include "game.h"

/* the ladder's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_0265A4 = 0x0265A4u;

enum { BOSS2_LADDER_BLOCKS = 6 };   /* main + 5 extra blocks (+$10(a6) = the ladder block) */

/* Boss_Stage2_SpawnEndLadder - boss native at the end of the defeat: spawns the rope ladder
 * hung from the wreck (Scr2_0265A4 on the ladder-thug body $0265F2, 6 blocks), whose coroutine
 * waits $B40 frames and then starts the section exit (Section_StartExitN, timer $530)
 * In:  x/y = where the ladder hangs
 * Out: d7 = 6, then Obj_SpawnEnemyNOrQuit (a short pool returns past this to the script op) and
 *      Enemy_LadderThug_InitLadder on the new ladder (a6): d0 as it leaves it
 * Evidence: lea Scr2_0265A4,a2 / move.w #6,d7 / jsr Obj_SpawnEnemyNOrQuit / jmp
 *           Enemy_LadderThug_InitLadder; op $2CC4 at $025DFA; TAS f34742 (one frame); the exit to
 *           the flying section follows at f35416. The prototype takes &a6 (measured a clobber: no
 *           caller reads it) so the jump hands the new ladder on to Enemy_LadderThug_InitLadder
 */
void Boss_Stage2_SpawnEndLadder(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint16_t *d7, uint32_t a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0, a3-a5 itself: the values going in are not read */
    uint32_t list = 0, motion = 0, vm = a4, sprite = 0;
    *d7 = BOSS2_LADDER_BLOCKS;
    Obj_SpawnEnemyNOrQuit(d0, d1, x, y, d7, &list, Scr2_0265A4, &motion, &vm, &sprite, a6);
    dk::jump(at_Enemy_LadderThug_InitLadder);   /* its a6: the new ladder */
}
