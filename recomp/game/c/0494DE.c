#include "game.h"

/* the game-over director's a2 script (ROM) */
static constexpr uint32_t Scr2_0494EA = 0x0494EAu;

/* GameOver_SpawnDirector - spawns the game-over director (Scr2_0494EA): ends the scene after 1000
 * frames, flickers the fade level at random, spawns snowflake effects at the screen centre and
 * scrolls the city (GameOver_ScrollCity) every frame.
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: Obj_SpawnEnemy's results (a6 = the object, a3/a4 its blocks)
 * Evidence: lea Scr2_0494EA,a2 / jmp Obj_SpawnEnemy; Scr2_0494EA: CallNative Scene_EndAfterDelay
 *           0/$3E8, coroutines $4950C (Fade_FlickerTarget) and $4951E (Op2_FxPosScreenCentre,
 *           Op2_FxSpawnRegs), CallNativeRel $49606; only caller GameOver_Run;
 *           scenario-game-over f42833
 */
void GameOver_SpawnDirector(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* a jmp, called: a2 (the script) is not this prototype's to hand on; Obj_SpawnEnemy plays no
       stack tricks */
    uint32_t d0 = 0, d7 = 0, a0 = 0, a5 = 0;
    Obj_SpawnEnemy(&d0, d1, x, y, &d7, &a0, Scr2_0494EA, a3, a4, &a5, a6);
}
