#include "game.h"

static constexpr uint16_t STREET_SEGMENT_DIV = 0xB400;            /* $168 px x $80: the facade x >> 9 per segment */
static constexpr int32_t STREET_ROUND_AT = (int32_t)0xFF4C0000;   /* -$B4 px: past half a segment, round up */
static constexpr uint32_t STREET_SEGMENT = 0x1680000u;            /* $168 px, 16.16 */

/* divu.w: quotient in the low word, remainder in the high word; on overflow the register stays */
static uint32_t divu_w(uint32_t d, uint16_t s)
{
    uint32_t q = d / s, r = d % s;
    if (q > 0xFFFF) return d;
    return (r << 16) | q;
}

/* Stage1_UpdateStreetScroll - per-frame scroll of the stage 1 street: facade x = g_ViewX * 35/32,
 * the current $168-px facade segment (shifting the sign slots when it advances), the plane B line
 * scroll, then the plane A fire sway or sky wave
 * In:  g_ViewX; d2-d4/d6 and a1 pass on to the line routines (Mem_FillRamp's second ramp)
 * Out: the facade x (Stage1StreetVars), g_Stage1SegOfs, g_Stage1Segment; a new segment moves the
 *      sign slots 2-3's wanted panels into slots 0-1 and clears 2-3; Stage1_ScrollFacadeLines,
 *      then Stage1_WaveSkyLines (state 0) or Stage1_SwayFireLines (> 0) or nothing (< 0: the boss)
 * Evidence: x + x/8 - x/32 (lsr.l #3 / lsr.l #2) / move.l d0,$F506 / lsr.l #9 / divu.w #$B400 /
 *           clr.w d0 / lsr.l #7 / neg.l / cmpi.l #$FF4C0000 / bgt / addq.w #1,d1 / addi.l #$1680000 /
 *           cmp.w g_Stage1Segment,d1 / ble / move.w $F4FC,$F4FA / clr.w $F4FC / bsr
 *           Stage1_ScrollFacadeLines / tst.b g_Stage1FireState / beq.w Stage1_WaveSkyLines / bpl.w
 *           Stage1_SwayFireLines / rts; called every frame by the street loop $1FB3A, TAS f532-f6334
 */
void Stage1_UpdateStreetScroll(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d6, uint32_t a1)
{
    Stage1StreetVars *vars = (Stage1StreetVars *)g_SceneVars;
    uint32_t x = g_ViewX;
    uint32_t eighth = x >> 3;
    uint32_t facade = x + eighth - (eighth >> 2);   /* x * 35/32 with the asm's truncations */
    vars->facade_x = facade;
    uint32_t div = divu_w(facade >> 9, STREET_SEGMENT_DIV);
    uint16_t segment = (uint16_t)div;
    int32_t offset = -(int32_t)((div & 0xFFFF0000u) >> 7);   /* the remainder as minus the 16.16 offset */
    if (offset <= STREET_ROUND_AT) {
        segment++;
        offset += (int32_t)STREET_SEGMENT;
    }
    g_Stage1SegOfs = (uint32_t)offset;
    *d1 = ((eighth >> 2) & 0xFFFF0000u) | segment;
    if ((int16_t)segment > (int16_t)g_Stage1Segment) {
        g_Stage1Segment = segment;
        vars->sign_wanted[0] = vars->sign_wanted[2];   /* the slots move on by two */
        vars->sign_wanted[1] = vars->sign_wanted[3];
        vars->sign_wanted[2] = 0;
        vars->sign_wanted[3] = 0;
    }
    uint16_t lines = (uint16_t)*d6;
    Stage1_ScrollFacadeLines(d1, d2, d3, d4, &lines, a1);
    *d6 = (*d6 & 0xFFFF0000u) | lines;
    int8_t state = (int8_t)g_Stage1FireState;
    if (state == 0) {
        dk::jump(at_Stage1_WaveSkyLines);
        return;
    }
    if (state > 0) {
        dk::jump(at_Stage1_SwayFireLines);
        return;
    }
}
