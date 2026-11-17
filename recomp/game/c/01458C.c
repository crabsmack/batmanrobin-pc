#include "game.h"

/* One item of a settings menu's table (after its first word, the last item's index). */
struct MenuSetting {
    PTR(u16) value;        /* +0: the setting word */
    PTR(const u16) max;    /* +4: its maximum */
};
static_assert(sizeof(MenuSetting) == 8, "MenuSetting");

/* Menu_Update - a settings menu's input: moves the cursor, then Right / Left step the selected
 * setting through 0..its maximum, wrapping around.
 * In:  table = a word, the last item, then a MenuSetting per item
 * Out: d1 = the last item; d0/pad as Menu_MoveCursor, then d0 = the cursor x 8 (low word), then the
 *      setting (low word; moveq #0 on a wrap to 0); value_at = the setting's address; the setting
 * Evidence: move.w (a0)+,d1 / bsr Menu_MoveCursor / asl.w #3,d0 / adda.w d0,a0 / movea.l (a0)+,a1 /
 *           movea.l (a0)+,a0 / move.w (a1),d0 / btst #3,d2 (right) addq / cmp.w (a0),d0 / ble / moveq
 *           #0; btst #2 (left) subq / bpl / move.w (a0),d0; caller $499FC with Tbl_049994 (options:
 *           the music / sound test numbers $F60A/$F60C); scenario-options
 */
void Menu_Update(uint32_t *d0, uint16_t *last, uint8_t *pad, uint32_t table, uint32_t *value_at)
{
    PTR(const u16) at;
    at.v = table;
    const u16 *t = at.get();
    *last = *t;
    Menu_MoveCursor(d0, *last, pad);
    uint16_t offset = (uint16_t)(*d0 << 3);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    const MenuSetting *items = (const MenuSetting *)(t + 1);
    const MenuSetting *item = &items[(int16_t)offset >> 3];   /* adda.w: the offset sign-extended */
    *value_at = item->value.raw();
    u16 *value = item->value;
    const u16 *max = item->max;
    uint16_t v = *value;
    *d0 = (*d0 & 0xFFFF0000u) | v;
    if (*pad & 0x08) {                       /* right */
        v = (uint16_t)(v + 1);
        *d0 = (*d0 & 0xFFFF0000u) | v;
        if ((int16_t)v > (int16_t)(uint16_t)*max)
            *d0 = 0;
        *value = (uint16_t)*d0;
    } else if (*pad & 0x04) {                /* left */
        v = (uint16_t)(v - 1);
        *d0 = (*d0 & 0xFFFF0000u) | v;
        if ((int16_t)v < 0)
            *d0 = (*d0 & 0xFFFF0000u) | (uint16_t)*max;
        *value = (uint16_t)*d0;
    }
}
