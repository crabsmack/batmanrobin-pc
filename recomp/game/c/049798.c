#include "game.h"

/* the 7 channels of the fade-in (ROM): delay, start level, then the colour range each one lights */
static constexpr uint32_t Tbl_04984C = 0x04984Cu;

/* a work-RAM address as the 68000 forms it with an absolute long (lea ($FF0008).l: $00FF0008) */
static uint32_t ram24(uint32_t a) { return a << 8 >> 8; }

/* Title_StartPaletteFade - creates the title's palette fade-in object (handler
 * Title_PaletteFadeRun): its palette pointer = the title palette in the scene buffer, and 7
 * channels from Tbl_04984C, each starting at level -$10 after its own delay.
 * In:  self = Obj_NewClear's a6 (passed through)
 * Out: the new object; d0 = Obj_NewClear's result; d7 = $FFFF (the dbra count)
 * Evidence: lea Title_PaletteFadeRun,a0 / jsr Obj_NewClear / lea ($FF0008).l,a0 / adda.l (a0),a0 /
 *           addq.l #2,a0 / move.l a0,$A(a6), then 7 x (delay, level, clr period, record) into
 *           $E(a6); only caller Title_Run; every title pass (TAS f404)
 */
void Title_StartPaletteFade(uint32_t *d0, uint32_t *d7, uint32_t self)
{
    uint32_t a6 = self;
    Obj_NewClear(d0, at_Title_PaletteFadeRun, &a6);
    PTR(TitleFadeObj) obj;
    obj.v = a6;

    /* the scene buffer begins with self-relative long offsets; the title's third entry (+8) is
       the palette, after one word */
    const u32 *entry = (const u32 *)g_SceneBuf + 2;
    obj->palette.v = ram24(dk::addr(entry)) + *entry + 2;

    PTR(const TitleFadeEntry) tbl;
    tbl.v = Tbl_04984C;
    for (int i = 0; i < 7; i++) {
        TitleFadeChannel *ch = &obj->ch[i];
        const TitleFadeEntry *e = &tbl.get()[i];
        ch->delay = e->delay;
        ch->level = e->level;
        ch->period_count = 0;
        ch->range = &e->range;
    }
    *d7 = 0x0000FFFFu;   /* moveq #6,d7 counted down by dbra */
}
