#include "game.h"

/* the pad record's newly-pressed byte (+2): the buttons the debug pause menu reads */
enum {
    PAD_B = 0x10,
    PAD_C = 0x20,
    PAD_A = 0x40,
    PAD_START = 0x80,
};

/* Debug_PauseMenu - the debug pause loop (Pause_Update jumps here while g_DebugPause is set): waits
 * for a press on the pausing player's pad: A toggles slow motion, B the free camera (placing the
 * players at its cursor when it turns off, centring the cursor when it turns on), C ends the
 * section, Start resumes. The pad record is refreshed by the V-int while it waits.
 * In:  pad = the pausing player's live pad record (+2 = newly pressed)
 * Out: g_FrameBusy set while waiting, cleared at the end; g_DebugPadPtr; A: g_SlowMotion
 *      toggled; B: g_FreeCam toggled, then Debug_CentreCursor (on) or player = g_Player1, then
 *      g_Player2, through Debug_PlacePlayerAtCursor (off); C: g_FrameCallback's high word = 1, then
 *      Scene_FadeOutExit
 * Evidence: st.b (g_FrameBusy).w / move.w a0,(g_DebugPadPtr).w / btst #6/#4/#5/#7,$2(a0) / beq
 *           Debug_PauseMenu; not.b g_SlowMotion; not.b g_FreeCam / bne Debug_CentreCursor; move.w #1,
 *           $F604.w / jmp Scene_FadeOutExit; jmp from Pause_Update ($9C50); never run in the corpus
 *           (g_DebugPause has no writer but the boot clear)
 */
void Debug_PauseMenu(const u8 *pad, uint32_t *player)
{
    for (;;) {
        g_FrameBusy = 0xFF;
        g_DebugPadPtr = (uint16_t)dk::addr(pad);
        if (pad[2] & PAD_A) {
            g_SlowMotion = (uint8_t)~g_SlowMotion;
            g_FrameBusy = 0;
            return;
        }
        if (pad[2] & PAD_B) {
            g_FrameBusy = 0;
            g_FreeCam = (uint8_t)~g_FreeCam;
            if (g_FreeCam) {
                dk::jump(at_Debug_CentreCursor);
                return;
            }
            *player = dk::addr(g_Player1);
            Debug_PlacePlayerAtCursor(player);
            *player = dk::addr(g_Player2);
            dk::jump(at_Debug_PlacePlayerAtCursor);   /* falls into it (the next file) */
            return;
        }
        if (pad[2] & PAD_C) {
            g_FrameCallback = (1u << 16) | (g_FrameCallback & 0xFFFFu);   /* move.w #1: the high word */
            g_FrameBusy = 0;
            dk::jump(at_Scene_FadeOutExit);
            return;
        }
        if (pad[2] & PAD_START) {
            g_FrameBusy = 0;
            return;
        }
        IRQ_WAIT();                                   /* the V-int refreshes the pad record */
    }
}
