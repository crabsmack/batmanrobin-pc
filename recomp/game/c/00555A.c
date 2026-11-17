#include "game.h"

/* Op4_KillOffScreen / Op4_ReplaceSelf ($00555A-$005585). */

/* the on-screen window of the a4 objects: x - g_OnScreenLeft < $170 and y - g_OnScreenTop < $110
   (unsigned: both edges in one compare); the draw op's tail $67CA tests the same */
static constexpr uint16_t OP4_ONSCREEN_W = 0x170;
static constexpr uint16_t OP4_ONSCREEN_H = 0x110;

/* Op4_KillOffScreen - a4 op: deletes the object when its drawn point is outside the on-screen
 * window; otherwise continues.
 * In:  draw_x/draw_y = d4/d5 (the drawn point's words after a motion op); script = the next op;
 *      d2/a0/a1 as Op4_Kill takes them
 * Out: d0 = the coordinate tested last; the condition codes of its cmpi; Op4_Kill when outside,
 *      else NEXT
 * Evidence: move.w d4,d0 / sub.w (g_OnScreenLeft).w,d0 / cmpi.w #$170,d0 / bcc Op4_Kill / the same
 *           for d5, g_OnScreenTop and $110 / NEXT; script use $26D6A after $61FC; 3 corpus runs (fuzz)
 */
void Op4_KillOffScreen(uint16_t *d0, uint16_t *d2, uint16_t draw_x, uint16_t draw_y, uint32_t *a0, uint32_t *a1,
                       const u16 **script, uint32_t a6, dk::Flags *ccr)
{
    (void)d2, (void)a1, (void)a6;
    *d0 = (uint16_t)(draw_x - g_OnScreenLeft + WS_WIN_L());   /* native: the window widened to the view (R4, R20) */
    *ccr = dk::Flags::compare(*d0, (uint16_t)(OP4_ONSCREEN_W + (WS_WIN_L() + WS_WIN_R())), 2, ccr->x);
    if (*d0 >= (uint16_t)(OP4_ONSCREEN_W + (WS_WIN_L() + WS_WIN_R()))) {
        dk::jump(at_Op4_Kill);
        return;
    }
    *d0 = (uint16_t)(draw_y - g_OnScreenTop);
    *ccr = dk::Flags::compare(*d0, OP4_ONSCREEN_H, 2, ccr->x);
    if (*d0 >= OP4_ONSCREEN_H) {
        dk::jump(at_Op4_Kill);
        return;
    }
    *a0 = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*a0);
}

/* Op4_ReplaceSelf - a4 op: turns the object into a new one in place: VM4_ReinitSelf drops its
 * collision box and restarts its script and motion from the spawn operands (d2-d5 plus an
 * optional parameter table); its rts goes to VM4_ResumeA5, which runs the new script now.
 * In:  a4 = the spawn operands (failure offset, script offset, table offset); d2-d5; a6 = object
 * Out: a5 = a6; (sp) = VM4_ResumeA5; jumped to VM4_ReinitSelf
 * Evidence: movea.l a6,a5 / pea (VM4_ResumeA5).l / bra.w VM4_ReinitSelf; script uses $14C3E, $151D4
 *           (after $63A2 loads the own position); 147 corpus runs
 */
void Op4_ReplaceSelf(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0,
                     uint32_t a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)a0, (void)a1, (void)a3, (void)a4;
    *a5 = a6;
    dk::push_long(at_VM4_ResumeA5);
    dk::jump(at_VM4_ReinitSelf);
}
