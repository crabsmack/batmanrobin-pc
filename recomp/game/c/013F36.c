#include "game.h"

/* the a2 script after the jsr $7FE.w: Op2_Kill (batmanrobin.asm) */
static constexpr uint32_t Scr2_013F3E = 0x013F3Eu;

/* WeaponDrop_Stop - the director's stop command: no director any more (g_WeaponDropper = 0), then
 * the object runs the script after the jsr, which kills it.
 * In:  the director's registers (entered through Op2_Native at $13F34, the stop script)
 * Out: g_WeaponDropper = 0; then VM2_Enter on Scr2_013F3E (Op2_Kill) with every register as it came
 * Evidence: clr.w (g_WeaponDropper).w / jsr ($7FE).w; $13F34 is the stop script given by the trigger
 *           at $2355C and the natives at $29334/$30144/$3D0FA; 6 TAS frames (46593-114869)
 */
void WeaponDrop_Stop(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                     uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    g_WeaponDropper = 0;
    dk::jump_script(at_VM2_Enter, Scr2_013F3E);
}
