#include "game.h"

/* the rabbit's script after the native (the bytes after its jsr $7FE.w): Op2_Kill */
static constexpr uint32_t Scr2_035E96 = 0x035E96u;

enum {
    BOMB_RABBIT_BLAST_GAP = 0x14,   /* frames every other bomb rabbit's explosion is held */
};

/* Enemy_BombRabbit_SetBlastGap - native at a bomb rabbit's explosion (after the blast effect): sets
 * g_ScriptTimer to 20 frames, which holds every other bomb rabbit's explosion until it runs out
 * (Enemy_BombRabbit_BlastRecent), then the script kills the rabbit
 * In:  the object's registers (for the script)
 * Out: g_ScriptTimer = $14; the script continues at Scr2_035E96 (jsr $7FE.w)
 * Evidence: move.w #$14,g_ScriptTimer / jsr ($7FE).w; a2 native word at $35E8A in Scr2_035D74;
 *           TAS f82709-f111908 (4 explosions)
 */
void Enemy_BombRabbit_SetBlastGap(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                  uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                                  uint32_t a6)
{
    g_ScriptTimer = BOMB_RABBIT_BLAST_GAP;
    dk::jump_script(at_VM2_Enter, Scr2_035E96);
}
