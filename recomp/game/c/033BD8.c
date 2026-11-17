#include "game.h"

/* the lamp's script after the native (the bytes after its jsr $7FE.w): cruise, Op2_SubLiftVelY */
static constexpr uint32_t Scr2_033BF4 = 0x033BF4u;

enum {
    LIFT_LAMP_SCREEN_Y = 0x0E,   /* screen-space offset of the lift shaft map, whole pixels (addi.l #$E0000) */
};

/* Stage2_LiftLampInitY - native at the start of the lift lamp's script: converts its trigger y
 * (whole pixels) to the scrolling map's screen space (- g_MapScrollY + g_LiftVelY + 14 px), then
 * continues the script
 * In:  self = the lamp object (a6: y +$42, 16.16); the rest the object's registers (for the script)
 * Out: self y = (y & $FFFF0000) - g_MapScrollY + g_LiftVelY + $E.0000; the script continues at
 *      Scr2_033BF4 (jsr $7FE.w)
 * Evidence: move.l $42(a6),d0 / clr.w d0 / sub.l g_MapScrollY,d0 / add.l g_LiftVelY,d0 /
 *           addi.l #$E0000,d0 / move.l d0,$42(a6) / jsr ($7FE).w; a2 native word $33BD8 in
 *           Scr2_033BB8; TAS f22282-f31762 (22 spawns). d0 is no output of the measured contract
 *           (the script never reads it)
 */
void Stage2_LiftLampInitY(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                          uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, Vm2Obj *self)
{
    uint32_t y = (uint32_t)(uint16_t)self->y << 16;   /* the fraction dropped */
    y = y - g_MapScrollY + g_LiftVelY + ((uint32_t)LIFT_LAMP_SCREEN_Y << 16);
    self->y = (int16_t)(y >> 16);
    self->y_frac = (uint16_t)y;
    dk::jump_script(at_VM2_Enter, Scr2_033BF4);
}
