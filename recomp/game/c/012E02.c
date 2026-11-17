#include "game.h"

/* Trigger_RunX - per gameplay frame: fires the triggers whose x key has come onto the screen
 * (g_CameraX to g_CameraX + $140), in key order from g_TriggerNext; keys already left of the
 * screen are skipped for good, and a callback that asks to be retried stops the walk there, to be
 * tried again next frame. Nothing while the screen-clear pause runs.
 * In:  g_TriggerNext, g_TriggerTable, g_CameraX; g_ScreenClearTimer
 * Out: g_TriggerNext = the first element not passed; left/right = the screen's x range (d6/d7 low
 *      words); next/table (a2/a3); d0 = the last element read, swapped (Trigger_Fire's); the
 *      callbacks' registers
 * Evidence: tst.w (g_ScreenClearTimer).w / move.l (a2)+,d0 / swap / cmp.w d7,d0 / bcc / cmp.w d6,d0
 *           / bcs / bsr Trigger_Fire / beq / subq.l #4,a2; the gameplay frames $9BC0/$9BD4 and the
 *           stage loops ($03334A, $0401DA, ...); 77k TAS frames
 */
void Trigger_RunX(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *left,
                  uint32_t *right, uint32_t *a0, uint32_t *a1, const TriggerKey **next, const u8 **table,
                  uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    if (g_ScreenClearTimer != 0)
        return;
    PTR(const TriggerKey) n;
    n.v = (uint32_t)g_TriggerNext;
    *next = n;
    PTR(const u8) t;
    t.v = (uint32_t)g_TriggerTable;
    *table = t;
    uint16_t x = (uint16_t)(g_CameraX >> 16);
    *left = (*left & 0xFFFF0000u) | x;
    *right = (*right & 0xFFFF0000u) | (uint16_t)(x + 0x140 + WS_WIN_R());   /* native: + the view right of 4:3 (R4, R20) */
    for (;;) {
        const TriggerKey *k = (*next)++;
        uint16_t key = k->key;
        *d0 = ((uint32_t)(uint16_t)k->offset << 16) | key;   /* move.l (a2)+,d0 / swap */
        if (key >= (uint16_t)*right)
            break;                                            /* not on screen yet */
        if (key < (uint16_t)*left)
            continue;                                         /* already behind the camera */
        dk::Flags fired = {};
        Trigger_Fire(d0, d1, d2, d3, d4, d5, *left, *right, a0, a1, ADDR68(*next), *table, a4, a5, a6, &fired);
        if (!fired.z)
            break;                                            /* retried next frame */
    }
    --*next;
    g_TriggerNext = ADDR68(*next);
}
