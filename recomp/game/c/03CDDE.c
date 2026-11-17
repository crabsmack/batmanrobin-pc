#include "game.h"

/* the spawned object's a2 script (ROM data right after this spawner) */
static constexpr uint32_t Scr2_03CDEA = 0x03CDEAu;

/* ToyFigure_SpawnFlyer - spawner (boss class) of a spinning toy figure flung out of a reveal burst:
 * anim $036CC8 (ToyFigure_DrawSpin3), palette 3, flies at cruise $355 along its heading, killed
 * off-screen; after $2D0 ticks it gets an attack box (EnemyAtk_HitKnockSpeed $20/$300) and aims at
 * the balanced player target
 * In:  d4/d5 = position (the burst spot, from Stage3_RevealBurst)
 * Out: jumps to Obj_SpawnBoss with a2 = Scr2_03CDEA
 * Evidence: jsr from Stage3_RevealBurst ($03CD54); Scr2_03CDEA: Op2_SetAnim -> $036CC8 (`2CB2 001C`
 *           = ToyFigure_DrawSpin3), Op2_KillOffscreen50, Op2_BalancePlayerTarget; TAS
 *           f97917-f100195 (36 frames = the 36 bursts)
 */
void ToyFigure_SpawnFlyer(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_03CDEA;
    dk::jump(at_Obj_SpawnBoss);
}
