#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_038BAA = 0x038BAAu;

/* ToyFigure_SpawnRandCharge - spawner of a single toy figure with a randomly timed charge cycle
 * (coroutine 3 $0387EE: random 1-2 s waits before each charge) and the pause-then-random-hop
 * coroutine $038D9A; body $036BCE
 * In:  d4/d5 = position (anchor)
 * Out: jumps to Obj_SpawnEnemy with a2 = Scr2_038BAA
 * Evidence: dc.l $038B9E in two chain shedding routines (Scr2_0386CC+$364, +$37A); in the corpus it
 *           ran (ctarget) but has no function-start frames in the TAS; a2 an output (measured as a
 *           clobber): Obj_SpawnEnemy, jumped to, takes the script in it
 */
void ToyFigure_SpawnRandCharge(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4,
                               uint32_t a6, uint32_t *script)
{
    *script = Scr2_038BAA;
    dk::jump(at_Obj_SpawnEnemy);
}
