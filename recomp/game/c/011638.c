#include "game.h"

static constexpr uint32_t Tbl_058768 = 0x058768u;   /* the ring's tile indices, frame A */
static constexpr uint32_t Tbl_0588CA = 0x0588CAu;   /* frame B */
static constexpr uint32_t Tbl_058A40 = 0x058A40u;   /* the lives digit -> tile index */
static constexpr uint32_t Art_058AA2 = 0x058AA2u;   /* the HUD's tiles */

/* HUD_DrawLifeRing - draws the 3x3-tile ring around the lives digit into g_HUDTileBuf: its
 * segments show health +$4 / 16 (0-16; the two frame tables alternate every 4 draws by the
 * counter +$24), then the lives digit (+$6, at most 9) goes over the centre tile: 3 of every 4
 * bytes of its 7 rows (the fourth keeps the ring's pixels).
 * In:  a6 = the player record
 * Out: 9 tiles at g_HUDTileBuf (3 per column), the digit at g_HUDTileBuf+$80; +$24 + 1; a0 = the
 *      digit's table, a1/a2 past the digit's rows, a3/a4 past the last ring copy, d6/d7 low words
 *      $FFFF (dbra)
 * Evidence: lea Tbl_058768,a0 / move.b $24(a6),d0 / addq.b #1 / btst #2 / lea Tbl_0588CA,a0 / move.b
 *           d0,$24 / move.w $4(a6),d0 / bpl / moveq #0 / lsr.w #4 / cmpi.w #$10 ble / x6 / adda.w;
 *           the copy loop lea $76(a3),a3 / addq.w #2,a0; lea Tbl_058A40 / lea $FFFFEAA4,a2 / cmpi.w
 *           #9 bcs / 7 x (move.w / move.b / addq.l #1 both); called by HUD_StepP1Ring /
 *           HUD_StepP2Ring
 */
void HUD_DrawLifeRing(uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                      uint32_t *a4, uint32_t a6)
{
    PTR(PlayerRec) p;
    p.v = a6;
    PlayerRec *rec = p;

    uint8_t anim = (uint8_t)(rec->ring_anim + 1);
    *a0 = (anim & 4) ? Tbl_058768 : Tbl_0588CA;
    rec->ring_anim = anim;
    int16_t health = (int16_t)(uint16_t)rec->health;
    uint16_t segments = health < 0 ? 0 : (uint16_t)((uint16_t)health >> 4);
    if ((int16_t)segments > 16)
        segments = 16;
    *a0 += (uint32_t)(int32_t)(int16_t)(uint16_t)(segments * 6);

    *a1 = Art_058AA2;
    *a2 = dk::addr(g_HUDTileBuf);
    *d6 = (*d6 & 0xFFFF0000u) | 2;
    for (int col = 0; col < 3; col++) {
        *d7 = (*d7 & 0xFFFF0000u) | 2;
        *a3 = *a0;
        for (int row = 0; row < 3; row++) {
            PTR(const u16) index;
            index.v = *a3;
            uint16_t offset = (uint16_t)(*index.get() << 5);
            *a4 = *a1 + (uint32_t)(int32_t)(int16_t)offset;
            PTR(const u8) src;
            src.v = *a4;
            PTR(u8) dst;
            dst.v = *a2;
            dk::mem_copy(dst.get(), src.get(), 32);
            *a4 += 32;
            *a2 += 32;
            *a3 += 0x76;
        }
        *a0 += 2;
    }
    *d6 |= 0xFFFFu;
    *d7 |= 0xFFFFu;

    *a0 = Tbl_058A40;
    *a1 = Art_058AA2;
    *a2 = dk::addr(g_HUDTileBuf + 0x80);
    uint16_t lives = rec->lives;
    if (lives >= 9)
        lives = 9;
    PTR(const u16) digit;
    digit.v = *a0 + lives * 2;
    *a1 += (uint32_t)(int32_t)(int16_t)(uint16_t)(*digit.get() << 5);
    PTR(const u8) src;
    src.v = *a1;
    PTR(u8) dst;
    dst.v = *a2;
    for (int row = 0; row < 7; row++)
        dk::mem_copy(dst.get() + row * 4, src.get() + row * 4, 3);
    *a1 += 7 * 4;
    *a2 += 7 * 4;
}
