#include "game.h"

/* The panel script's content ops ($0117C8-$011833): back to the stats, or a 12-character text
 * (HUD_StepP1Panel / HUD_StepP2Panel draw +$1A once and clear it). */

/* HudOp_ShowStats - panel script op $1C: leaves text mode, so the panel steps draw the score, bar
 * and ring again; runs on.
 * In:  a0 = the next op; rec = the player record (a6)
 * Out: +$25 bit 7 cleared; then HUD_PanelNext
 * Evidence: bclr #7,$25(a6) / bra.s HUD_PanelNext; entry $1C of Tbl_011764; first op of Tbl_010F84
 */
void HudOp_ShowStats(uint32_t *a0, uint32_t *a1, PlayerRec *rec)
{
    (void)a0, (void)a1;
    rec->flags = (uint8_t)(rec->flags & ~PLAYER_PANEL_TEXT);
    dk::jump(at_HUD_PanelNext);
}

/* HudOp_ShowText - panel script op $18: shows a 12-character text and waits.
 * In:  a0 = the operands: word frames, word text offset (from the op word); rec = the player
 *      record (a6)
 * Out: a1 = the text = +$1A; +$1E = the frames, +$25 bit 7 set, +$20 = the next op
 * Evidence: move.w (a0)+,$1E(a6) / move.w (a0)+,d0 / lea -6(a0,d0.w),a1 / move.l a1,$1A(a6) / bset
 *           #7,$25(a6) / move.l a0,$20(a6); entry $18 of Tbl_011764; texts ' GAME  OVER ',
 *           ' PRESS START', ' BOSS  TIME ', 'PLEASE WAIT ' at $10F38-$10F73
 */
uint32_t HudOp_ShowText(uint32_t a0, PlayerRec *rec)
{
    PTR(const u16) operand;
    operand.v = a0;
    const u16 *w = operand.get();
    rec->panel_timer = w[0];
    uint32_t text = a0 + 4 - 6 + (uint32_t)(int32_t)(int16_t)w[1];
    rec->panel_text = text;
    rec->flags = (uint8_t)(rec->flags | PLAYER_PANEL_TEXT);
    rec->panel_script = a0 + 4;
    return text;
}

/* HudOp_ShowCredits - panel script op $20: builds 'CREDIT nn  ' (g_Credits, at most 99) in
 * g_CreditText, shows it like HudOp_ShowText and waits.
 * In:  a0 = the operand (word frames); rec = the player record (a6); d0's upper word is the
 *      dividend's (divu.w divides all of d0: only the low word is loaded with g_Credits)
 * Out: a1 = past the 12 bytes; +$1A = g_CreditText, +$1E = the frames, +$25 bit 7 set, +$20 = the
 *      next op
 * Evidence: move.w (a0)+,$1E(a6) / lea g_CreditText,a1 / ... / move.l #'CRED',(a1)+ / move.l
 *           #'IT  ',(a1)+ / move.w g_Credits,d0 / cmpi.w #99 / divu.w #10,d0 / addi.b #'0' / swap /
 *           move.w #'  ',(a1)+; entry $20 of Tbl_011764; the GAME OVER loops $10F8A, $10FB2, $11012
 */
uint32_t HudOp_ShowCredits(uint32_t d0, uint32_t a0, PlayerRec *rec)
{
    PTR(const u16) operand;
    operand.v = a0;
    rec->panel_timer = *operand.get();
    u8 *text = (u8 *)g_CreditText;
    rec->panel_text = dk::addr(text);
    rec->flags = (uint8_t)(rec->flags | PLAYER_PANEL_TEXT);
    rec->panel_script = a0 + 2;
    dk::mem_copy(text, "CREDIT  ", 8);
    uint16_t credits = g_Credits;
    if (credits >= 99)
        credits = 99;
    d0 = (d0 & 0xFFFF0000u) | credits;
    uint32_t quotient = d0 / 10;
    if (quotient <= 0xFFFFu)            /* divu.w: an overflow leaves d0 as it was */
        d0 = ((d0 % 10) << 16) | quotient;
    text[8] = (uint8_t)(d0 + '0');
    text[9] = (uint8_t)((d0 >> 16) + '0');
    text[10] = ' ';
    text[11] = ' ';
    return dk::addr(text + 12);
}
