#include "game.h"

static constexpr uint32_t Tbl_058712 = 0x058712u;   /* digit -> tile index (a word per digit, x2) */
static constexpr uint32_t Art_058AA2 = 0x058AA2u;   /* the HUD's tiles */
static constexpr uint32_t HUD_SCORE_MAX = 999999;

/* HUD_DrawScore - draws the player's score (capped at 999999) as digit tiles in the panel's top
 * row, without leading zeros; each digit not drawn moves the start by d4 (0 for player 1's
 * left-aligned score, $60 = one column for player 2's right-aligned one).
 * In:  a6 = the player record (+0 long score); d4 = the column step per missing digit
 * Out: digit tiles from g_HUDTileBuf (column stride $60); a2 past the last, d7 = 6 - the digits
 *      (the columns HUD_ClearTiles pads), d2/d3 as the last digit's copy left them
 * Evidence: move.l (a6),d0 / cmpi.l #$F423F / lea Tbl_058712,a0 / lea Art_058AA2,a1 / lea
 *           g_HUDTileBuf,a2 / moveq #6,d7 / cmpi.l #100000 bcc / adda.w d4,a2 / cmpi.l #10000 /
 *           cmpi.w #1000 / #100 / #10, the bsr HUD_DrawScoreDigit chain d1 = 100000 .. 10, then
 *           move.w d0,d2 / add.w d2,d2 / bra.s HUD_CopyScoreDigit; called by HUD_StepP1Panel (d4 0)
 *           and HUD_StepP2Panel (d4 $60)
 */
void HUD_DrawScore(uint32_t *d2, uint32_t *d3, uint32_t d4, uint32_t *d7, uint32_t *a2, uint32_t a6)
{
    PTR(const PlayerRec) p;
    p.v = a6;
    uint32_t d0 = p.get()->score;
    if (d0 >= HUD_SCORE_MAX)
        d0 = HUD_SCORE_MAX;
    uint32_t a0 = Tbl_058712, a1 = Art_058AA2;
    *a2 = dk::addr(g_HUDTileBuf);
    *d7 = 6;
    uint32_t skip = (uint32_t)(int32_t)(int16_t)(uint16_t)d4;

    /* the first place value drawn: each one the score is below moves the start by a column step
       (the asm compares 1000 / 100 / 10 as words: the score is below 10000 by then) */
    static const uint32_t places[] = { 100000, 10000, 1000, 100, 10 };
    int first = 0;
    while (first < 5 && d0 < places[first]) {
        *a2 += skip;
        first++;
    }
    for (int i = first; i < 5; i++)
        HUD_DrawScoreDigit(&d0, places[i], d2, d7, a0, a1, a2);

    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)((uint16_t)d0 * 2);
    HUD_CopyScoreDigit(*d2, d3, d7, a0, a1, a2);   /* bra.s: a0/a1 are clobbers of this contract */
}
