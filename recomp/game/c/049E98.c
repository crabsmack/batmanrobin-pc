#include "game.h"

/* the logo's colour cycle (ROM): $6E bytes of colours, 11 shown at a time */
static constexpr uint32_t Tbl_049EDC = 0x049EDCu;

enum { CYCLE_BYTES = 0x6E, SHOWN_COLOURS = 11, FIRST_COLOUR = 5 };

/* ClockworkLogo_ScrollPicture - puts the Clockwork Tortoise picture (plane A) at the logo object's
 * y: V-scroll A = -y.
 * In:  self = the logo object
 * Out: g_VScrollA = -y, also returned
 * Evidence: move.w $42(a6),d0 / neg.w d0 / move.w d0,g_VScrollA; Op2_CallNativeRel $2C from $49E6A
 *           every frame; idle f3678-f35341
 */
uint16_t ClockworkLogo_ScrollPicture(const Vm2Obj *self)
{
    uint16_t v = (uint16_t)-self->y;
    g_VScrollA = v;
    return v;
}

/* ClockworkLogo_CycleColours - steps the logo's colour cycle: script variable 0 advances 2 bytes
 * (wrapping at $6E to 0) and the 11 colours from that offset of Tbl_049EDC become palette line 3
 * colours 5-15.
 * In:  vm = the logo object's a4 block (var 0 = the cycle offset)
 * Out: g_PalSource3 colours 5-15, g_Pal3Reload set; offset = the new offset (moveq #0 on the wrap:
 *      the whole register), dest = past colour 15, d7 = $FFFF (the dbra count)
 * Evidence: lea Tbl_049EDC,a0 / lea g_PalSource3+$A,a1 / move.w $22(a4),d0 / addq.w #2 / cmpi.w #$6E
 *           blt / moveq #0 / adda.w d0,a0 / moveq #$A,d7 / move.w (a0)+,(a1)+ dbra / st.b
 *           g_Pal3Reload; Op2_CallNativeRel $3C from $49E66; runtime entry in idle
 */
void ClockworkLogo_CycleColours(uint32_t *offset, uint32_t *d7, uint32_t *dest, Vm2Block *vm)
{
    uint16_t at = (uint16_t)(vm->var[0] + 2);
    if ((int16_t)at < CYCLE_BYTES)
        *offset = (*offset & 0xFFFF0000u) | at;
    else
        *offset = 0;
    vm->var[0] = (uint16_t)*offset;

    PTR(const u16) cycle;
    cycle.v = Tbl_049EDC + (uint32_t)(int32_t)(int16_t)*offset;
    u16 *line3 = (u16 *)g_PalSource3;
    for (int i = 0; i < SHOWN_COLOURS; i++)
        line3[FIRST_COLOUR + i] = cycle.get()[i];
    *dest = dk::addr(line3 + FIRST_COLOUR + SHOWN_COLOURS);
    *d7 = 0x0000FFFFu;   /* moveq #$A,d7 counted down by dbra */
    g_Pal3Reload = 0xFF;
}
