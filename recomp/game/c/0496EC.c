#include "game.h"

/* the director's script after the native (the bytes after its jsr $7FE.w) */
static constexpr uint32_t Scr2_049700 = 0x049700u;

enum { PAD_PRESSED = 2 };   /* g_Pad1 +2: the buttons newly pressed this frame */

/* Title_ReadPadAndCursor - title director per-frame native: the menu cursor into script variable 1
 * and pad 1's newly pressed buttons into variable 0, for the menu coroutines' branches; then back
 * into the script.
 * In:  vm = the director's a4 block; the rest the running object's registers (for the script)
 * Out: vm->var[1] = g_MenuCursor, vm->var[0] = d0 = g_Pad1 newly pressed (zero-extended); the script
 *      continues at Scr2_049700 (jsr $7FE.w)
 * Evidence: move.w g_MenuCursor,$24(a4) / moveq #0,d0 / move.b g_Pad1+2,d0 / move.w d0,$22(a4) /
 *           jsr $7FE.w; the coroutines test var 0 bit 7 (Start) and var 1 = 1 (OPTIONS); every
 *           title frame
 */
void Title_ReadPadAndCursor(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                            uint32_t d7, uint32_t a1, uint32_t a3, Vm2Block *vm, uint32_t a5, uint32_t a6)
{
    vm->var[1] = g_MenuCursor;
    *d0 = g_Pad1[PAD_PRESSED];   /* moveq #0 then move.b: zero-extended */
    vm->var[0] = (uint16_t)*d0;
    dk::jump_script(at_VM2_Enter, Scr2_049700);
}
