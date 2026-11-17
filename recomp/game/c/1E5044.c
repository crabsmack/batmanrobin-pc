#include "game.h"

/* Snd_SendSfx - driver API stub the V-int calls every full frame: hands the pending sound-effect
 * requests to the Z80 (Snd_SendSfxBody with a0 = g_SndSfxSlots)
 * In:  none (the V-int holds the Z80 bus already)
 * Out: as Snd_SendSfxBody
 * Evidence: move.l a1,-(a7) / lea g_SndSfxSlots,a0 / lea Snd_SendSfxBody,a1 / jsr (a1) /
 *           movea.l (a7)+,a1 / rts; only caller VInt_Handler $77B8 between its own bus request
 *           and release; 146 corpus runs
 */
void Snd_SendSfx(void)
{
    Snd_SendSfxBody(g_SndSfxSlot);
}
