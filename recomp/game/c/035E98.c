#include "game.h"

/* Enemy_BombRabbit_BlastRecent - switch native of a lit bomb rabbit's fuse: 1 while g_ScriptTimer
 * runs (another rabbit exploded less than 20 frames ago: keep ticking), else 0 (it may explode
 * when its fuse timer runs out)
 * In:  -
 * Out: d0 = 1 (the whole long) when g_ScriptTimer > 0 (signed); else d0's low word = g_ScriptTimer
 *      (0 or negative; the high word kept)
 * Evidence: move.w g_ScriptTimer,d0 / ble.s / moveq #1,d0 / rts; `2D30 0024 0004 0006` at $35E72
 *           (Op2_CallNativeRelSwitch); TAS f82646-f115353 (247 frames)
 */
void Enemy_BombRabbit_BlastRecent(uint32_t *d0)
{
    uint16_t timer = g_ScriptTimer;
    *d0 = (*d0 & 0xFFFF0000u) | timer;
    if ((int16_t)timer > 0)
        *d0 = 1;
}
