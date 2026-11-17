#include "game.h"

/* WeaponDrop_CommandNative - a2 native (op $2CBC): hands the weapon-drop director the command
 * script named by the dc.l after the op, spawning the director if there is none. a2-a6 are kept
 * across the command (saved on the 68000's stack, as the original's movem).
 * In:  script = at the dc.l (the command script's address); a4/a6 = the running object's blocks
 * Out: script past the long; command_at = past it too (WeaponDrop_Command's a1); WeaponDrop_Command's
 *      d0/d1/d7 and condition codes
 * Evidence: movea.l a2,a1 / addq.l #4,a2 / movem.l a2-a6,-(a7) / bsr WeaponDrop_Command / movem.l
 *           (a7)+,a2-a6 / rts; op $2CBC pointers at $29292/$29334/$30144/$30162/$3D03C/$3D0FA with
 *           scripts $14036/$14058 (drop) or $13F34 (stop); 10 TAS frames (70456-114869)
 */
void WeaponDrop_CommandNative(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7,
                              uint32_t *command_at, const PTR(const u16) **script, uint32_t a4, uint32_t a6)
{
    const PTR(const u16) *command = *script;   /* a1 stays a plain register: its input may be no address */
    (*script)++;
    /* movem.l a2-a6,-(a7): a3 and a5 are not inputs (nothing reads them back) */
    uint32_t a2 = dk::addr(*script), a3 = 0, a5 = 0;
    dk::push_long(a6);
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    uint32_t a0 = 0;
    WeaponDrop_Command(d0, d1, d4, d5, d7, &a0, &command, &a2, &a3, &a4, &a5, &a6);
    *command_at = dk::addr(command);
    for (int i = 0; i < 5; i++)
        dk::pop_long();                      /* movem.l (a7)+,a2-a6: the entry values stand */
}
