#include "game.h"

/* One SAT entry as the HUD's piece tables hold it (Tbl_011974 / Tbl_0119AC): the panel's y and x
 * are added; the link byte is replaced by the running sprite count. */
typedef struct HudSatEntry {
    u16 y;                 /* +0: relative to the panel's y */
    u16 size_link;         /* +2: size (high byte), link (low byte) */
    u16 attr;              /* +4: tile and attributes */
    u16 x;                 /* +6: relative to the panel's x */
} HudSatEntry;
static_assert(sizeof(HudSatEntry) == 8, "HudSatEntry");

/* the table's layout: the Batman / Robin portrait, 3 stats pieces, 2 text-mode pieces */
enum { HUD_PIECE_PORTRAIT = 0, HUD_PIECE_STATS = 2, HUD_PIECE_TEXT = 5 };

/* HUD_DrawPanelSprites - writes a player's HUD panel sprites straight into the SAT buffer: the
 * portrait (Batman, or Robin with +$25 bit 3) and the 3 stats pieces, or in text mode (bit 7) the 2
 * text pieces, each at the panel's position (+$C y, +$E x).
 * In:  a0 = the piece table (Tbl_011974 player 1, Tbl_0119AC player 2); a6 = the player record
 * Out: g_SATPtr / g_SprCount advanced (the count's low byte steps: addq.b); d0-d3 = the last
 *      entry written (movem.w: sign-extended, then the .w adds), d4 = y, d5 = x, d6 = the count,
 *      d7 = $FFFF (the dbra's end); a0 past the pieces read, a1 = the portrait's end (stats mode),
 *      a2 = the SAT write pointer
 * Evidence: movea.w (g_SATPtr).w,a2 / move.w $C(a6),d4 / move.w $E(a6),d5 / move.w g_SprCount,d6 /
 *           btst #7 / lea $28(a0),a0 / moveq #1,d7 or movea.l a0,a1 / lea $10(a0),a0 / btst #3 /
 *           addq.l #8,a1 / movem.w (a1)+,d0-d3 / moveq #3,d7; loop add.w d4,d0 / add.w d5,d3 /
 *           addq.b #1,d6 / move.b d6,d1 / movem.w d0-d3,(a2) / addq.l #8,a2 / dbra; called by
 *           HUD_Update for both players every frame
 */
void HUD_DrawPanelSprites(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a6)
{
    PTR(PlayerRec) p;
    p.v = a6;
    PlayerRec *rec = p;
    *a2 = (uint32_t)(int32_t)(int16_t)(uint16_t)g_SATPtr;
    *d4 = (*d4 & 0xFFFF0000u) | (uint16_t)rec->hud_y;
    *d5 = (*d5 & 0xFFFF0000u) | (uint16_t)((uint32_t)rec->panel_x >> 16);
    *d6 = (*d6 & 0xFFFF0000u) | (uint16_t)g_SprCount;

    PTR(const HudSatEntry) piece;
    int count;
    if (rec->flags & PLAYER_PANEL_TEXT) {
        piece.v = *a0 + HUD_PIECE_TEXT * sizeof(HudSatEntry);
        count = 2;
    } else {
        /* the portrait comes from a1 (a0 + 8 for Robin), the stats pieces from a0 */
        *a1 = *a0 + ((rec->flags & PLAYER_ROBIN) ? sizeof(HudSatEntry) : 0);
        piece.v = *a0 + HUD_PIECE_STATS * sizeof(HudSatEntry);
        count = 4;
    }
    *d7 = (uint32_t)(count - 1);

    PTR(HudSatEntry) out;
    out.v = *a2;
    for (int i = 0; i < count; i++) {
        PTR(const HudSatEntry) src;
        if (i == 0 && !(rec->flags & PLAYER_PANEL_TEXT)) {
            src.v = *a1;
            *a1 += sizeof(HudSatEntry);
        } else {
            src = piece;
            piece.v = piece.v + sizeof(HudSatEntry);
        }
        const HudSatEntry *e = src.get();
        *d0 = (uint32_t)(int32_t)(int16_t)(uint16_t)e->y;   /* movem.w: sign-extended */
        *d1 = (uint32_t)(int32_t)(int16_t)(uint16_t)e->size_link;
        *d2 = (uint32_t)(int32_t)(int16_t)(uint16_t)e->attr;
        *d3 = (uint32_t)(int32_t)(int16_t)(uint16_t)e->x;
        *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)(*d0 + *d4);
        *d3 = (*d3 & 0xFFFF0000u) | (uint16_t)(*d3 + *d5);
        *d6 = (*d6 & 0xFFFFFF00u) | (uint8_t)(*d6 + 1);
        *d1 = (*d1 & 0xFFFFFF00u) | (uint8_t)*d6;
        HudSatEntry *slot = out.get();
        slot->y = (uint16_t)*d0;
        slot->size_link = (uint16_t)*d1;
        slot->attr = (uint16_t)*d2;
        slot->x = (uint16_t)*d3;
        out.v = out.v + sizeof(HudSatEntry);
    }
    *d7 = (*d7 & 0xFFFF0000u) | 0xFFFFu;
    *a2 = out.v;
    *a0 = piece.v;
    g_SATPtr = (uint16_t)*a2;
    g_SprCount = (uint16_t)*d6;
}
