#include "game.h"

enum { PAD_HELD = 0, PAD_PRESSED = 2 };
enum { HALF_LINES = 112 };   /* each hero's half of the 224 screen lines */

/* PlayerSelect_ScrollHalves - player-select per-frame native: steps the line tint of the chosen
 * hero's palette line (1 Robin, 0 Batman: the confirm flashes), then fills plane A's line
 * H-scroll with the chooser's x for the top 112 lines (Batman) and its y word for the bottom 112
 * (Robin), so each half slides on its own.
 * In:  self = the chooser (x and y used as the two halves' scroll positions)
 * Out: g_HScrollLinesA filled (Mem_FillWords2); d0 = x, d2 = y, d7 = 112 going in; the tint
 *      step's d1
 * Evidence: tst.b g_P1IsRobin / Tint_StepLine1 or Tint_StepLine0 / move.w $3E(a6),d0 / $42(a6),d2 /
 *           lea g_HScrollLinesA,a0 / lea $E0(a0),a1 / move.w #$70,d7 / jmp Mem_FillWords2;
 *           Op2_CallNativeRel from $49B52 every frame; TAS f431-f476
 */
void PlayerSelect_ScrollHalves(uint16_t *d0, uint32_t *d1, uint16_t *d2, uint16_t *d7, uint32_t *a1,
                               const Vm2Obj *self)
{
    if (g_P1IsRobin) {
        uint8_t b0 = (uint8_t)*d0;
        dk::Flags cc{};   /* the condition codes nothing here reads */
        Tint_StepLine1(&b0, d1, &cc);
        *d0 = (uint16_t)((*d0 & 0xFF00u) | b0);
    } else {
        *d1 = Tint_StepLine0();
    }
    *d0 = (uint16_t)self->x;
    *d2 = (uint16_t)self->y;
    u16 *lines = (u16 *)g_HScrollLinesA;
    uint32_t a0 = dk::addr(lines);
    *a1 = dk::addr(lines + HALF_LINES);
    *d7 = HALF_LINES;
    Mem_FillWords2(*d0, d2, d7, &a0, a1);   /* a jmp: a0 isn't this prototype's to hand on */
}

/* PlayerSelect_ReadPad - player-select native: packs pad 1's newly pressed byte (high) and held
 * byte (low) into script variable 0 for the chooser's branches.
 * In:  vm = the chooser's a4 block
 * Out: vm->var[0] = d0 = newly pressed << 8 | held
 * Evidence: lea g_Pad1,a0 / move.b $2(a0),d0 / lsl.w #8,d0 / move.b (a0),d0 / move.w d0,$22(a4);
 *           Op2_CallNativeRel from $49B40 every frame; the coroutine at $49B5A tests var 0 & $8000
 *           (Start) and & $7300 (A/B/C, Up/Down)
 */
void PlayerSelect_ReadPad(uint16_t *d0, Vm2Block *vm)
{
    *d0 = (uint16_t)((g_Pad1[PAD_PRESSED] << 8) | g_Pad1[PAD_HELD]);
    vm->var[0] = *d0;
}
