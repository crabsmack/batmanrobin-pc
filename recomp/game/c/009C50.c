#include "game.h"

/* The pause ($009C50-$009CE5): Start pauses a stage; while paused the main loop spins with
 * g_FrameBusy set, so only the V-int's busy path refreshes the pads (Pad_ReadLive). A press of the
 * pausing pad unpauses on Start, else is checked against the section-skip code. */

/* the section-skip code: B A Down B A Down Left Up C, 0 ends */
static constexpr uint32_t Tbl_009CDC = 0x009CDCu;   /* the table after Pause_Update's rts */
static const u8 *skip_code(void)
{
    PTR(const u8) p;
    p.v = Tbl_009CDC;
    return p;
}

/* how the pause ended */
enum class PauseExit { None, Unpaused, DebugMenu };

/* the pause itself; pad / code are a0 / a1 as the asm moves them */
static PauseExit pause(const PadRec **pad, const u8 **code)
{
    if (!(g_SectionId >> 8))                      /* tst.b: the section id's high byte (the stage) */
        return PauseExit::None;
    if ((int8_t)g_FadeLevelVInt < 0)
        return PauseExit::None;
    *pad = (const PadRec *)g_PadLive1;
    *code = g_Player1;
    if (!(((const PlayerRec *)*code)->flags & 1) || !((*pad)->pressed & 0x80)) {
        *pad = (const PadRec *)g_PadLive2;
        *code = g_Player2;
        if (!(((const PlayerRec *)*code)->flags & 1))
            return PauseExit::None;
        if (!((*pad)->pressed & 0x80))
            return PauseExit::None;
    }
    g_FrameBusy = 0xFF;
    while (!((*pad)->released & 0x80))           /* $9C98: until Start is released */
        IRQ_WAIT();
    if (g_DebugPause)
        return PauseExit::DebugMenu;
    unsigned misses = 0;                          /* $9CBA's mismatches since the last V-int */
    for (;;) {
        uint8_t press;
        while ((press = (*pad)->pressed) == 0) {  /* $9CB0: a press */
            IRQ_WAIT();
            misses = 0;
        }
        if (press & 0x80)
            break;                                /* Start: unpause */
        if (press != *(*code)++) {
            /* $9CBA: a mismatch restarts the code, and the same press is compared with its first
             * byte in the same frame; only a second mismatch in a row spins until the V-int */
            if (++misses == 2) {
                IRQ_WAIT();
                misses = 0;
            }
            *code = skip_code();
            continue;
        }
        while ((*pad)->pressed != 0) {            /* $9CC0: the press released */
            IRQ_WAIT();
            misses = 0;
        }
        if (**code == 0) {                        /* the whole code: skip the section */
            g_JoinLocked = 0xFF;
            *(u16 *)&g_FrameCallback = 1;
            dk::Flags ccr{};
            Scene_FadeOutExit(&ccr);
            break;
        }
    }
    g_FrameBusy = 0;
    return PauseExit::Unpaused;
}

/* Pause_Update - in a stage, pauses when a player in play presses Start; holds the frame until
 * Start again (the debug pause menu instead when g_DebugPause is set). While paused, the code B A
 * Down B A Down Left Up C locks joining and ends the section (Scene_FadeOutExit).
 * In:  g_SectionId (0: no pause), g_FadeLevelVInt (negative: none); g_PadLive1/2, the players'
 *      in-play bits
 * Out: pad (a0) = the pausing pad's record (or the last looked at); code (a1) = the player record,
 *      or the next code byte; g_FrameBusy set while paused, clear after
 * Evidence: tst.b g_SectionId / tst.b g_FadeLevelVInt / btst #0,$25(a1) / btst #7,$2(a0); st.b
 *           g_FrameBusy; the wait sites $9C98 (Start released), $9CB0 (a press), $9CBA (a code
 *           mismatch restarts; a wait at the second in a row, spins = 2: the restart compares
 *           the same press with B first), $9CC0 (the press released); jmp Debug_PauseMenu; the first press is
 *           compared with the player record's first byte (a1 still points there: kept); move.w
 *           #1,g_FrameCallback then Scene_FadeOutExit overwrites the long
 */
void Pause_Update(uint32_t *pad_reg, uint32_t *code_reg)
{
    const PadRec *pad = nullptr;
    const u8 *code = nullptr;
    PauseExit how = pause(&pad, &code);
    if (pad) {                                    /* a0 / a1 set (else as they came) */
        *pad_reg = dk::addr(pad);
        *code_reg = dk::addr(code);
    }
    if (how == PauseExit::DebugMenu)
        dk::jump(at_Debug_PauseMenu);
}
