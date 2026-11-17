#include "game.h"

/* the object's a2 script (ROM) */
static constexpr uint32_t Scr2_04A5DC = 0x04A5DCu;

/* Ending_SpawnEpilogueNarrator - spawns the epilogue's narrator object (Scr2_04A5DC): its coroutine
 * waits 60 frames, types the text at $4ABDE, waits $7080 ticks and ends the scene; its main loop runs
 * Ending_EpilogueTextStep every frame
 * In:  x, y = the spawn position (Obj_SpawnEnemy's d4/d5)
 * Out: script = Scr2_04A5DC; Obj_SpawnEnemy's results (a6 = the object, d0 = 0 / -1)
 * Evidence: lea Scr2_04A5DC,a2 / jmp Obj_SpawnEnemy; Scr2_04A5DC: SetSpeed $100, StartCoroutine0Rel ->
 *           $4A602 (SetTimer0 $3C0, TextMap_RunInline $0004ABDE, SetTimer0 $7080, Scene_EndAfterDelay
 *           0), loop StepCoroutine0 / Op2_Native; f19910
 */
void Ending_SpawnEpilogueNarrator(uint32_t *d0, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *script,
                                  uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    *script = Scr2_04A5DC;
    dk::jump(at_Obj_SpawnEnemy);
}
