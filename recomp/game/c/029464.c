#include "game.h"

/* the ship's script after this native (the bytes after its jsr $7FE.w): award score, Op2_Kill */
static constexpr uint32_t Scr2_029472 = 0x029472u;

/* Boss_BeamShip_ReleaseRadiusObj - native at the end of the ship: links its (unlinked) radius
 * object onto the main object list so the death disc keeps running after the ship is killed
 * In:  ship = the ship (a6), a2 = its script; the rest its registers
 * Out: a1 = Obj_LinkAtEnd's result; the script continues at Scr2_029472 (jsr $7FE.w)
 * Evidence: movea.l $22(a6),a0 / jsr Obj_LinkAtEnd / jsr $7FE.w; Op2_Native at $029462; the
 *           radius object's death script then shrinks the disc and calls Section_StartExit; TAS
 *           f79029. The prototype takes &a1 (Obj_LinkAtEnd's output: the contract ends at the
 *           jsr) so the script gets it as on the 68000
 */
void Boss_BeamShip_ReleaseRadiusObj(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                                    uint32_t d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5,
                                    const BeamShipObj *ship)
{
    *a1 = Obj_LinkAtEnd(ship->radius.v, a2);
    dk::jump_script(at_VM2_Enter, Scr2_029472);
}
