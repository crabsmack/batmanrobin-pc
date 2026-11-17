#include "game.h"

/* the director's a2 script: coroutine 0 every frame (ROM data after this file; batmanrobin.asm) */
static constexpr uint32_t Scr2_013F2A = 0x013F2Au;

/* WeaponDrop_Command - gives the weapon-drop director (g_WeaponDropper) a command script in its
 * coroutine slot 0, spawning the director (a boss-class object running Scr2_013F2A) when there is
 * none.
 * In:  command = the long naming the command script (a trigger table entry or an a2 operand);
 *      a4/a6 = the running object's blocks (Obj_SpawnBossOrQuit's parent)
 * Out: the director's coroutine[0] = the script; g_WeaponDropper = the director; command past the
 *      long; director/vm = the director and its VM block; d0 = 0 (tst: Z). No director and a short
 *      pool: Obj_SpawnBossOrQuit returns past this with d0 nonzero.
 * Evidence: move.w (g_WeaponDropper).w,d0 / beq / movea.w d0,a6 / movea.w $A(a6),a4 / move.l
 *           (a1)+,$38(a4) / move.w a6,(g_WeaponDropper).w / moveq #0,d0; trigger callbacks (dc.l
 *           $13F02 then the script) at $163A4/$166E2/$16924/$21438/$21D46/$21DA4/$2355C; 17 TAS
 *           frames (4518-114869)
 */
void WeaponDrop_Command(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a0,
                        const PTR(const u16) **command, uint32_t *script, uint32_t *a3, uint32_t *vm, uint32_t *a5,
                        uint32_t *director)
{
    uint16_t dropper = g_WeaponDropper;
    *d0 = (*d0 & 0xFFFF0000u) | dropper;
    if (dropper) {
        *director = (uint32_t)(int32_t)(int16_t)dropper;                         /* movea.w */
        *vm = (uint32_t)(int32_t)dk::handle<const Vm2Obj>((int16_t)dropper)->vm;
    } else {
        *script = Scr2_013F2A;
        Obj_SpawnBossOrQuit(d0, d1, d4, d5, d7, a0, *script, a3, vm, a5, director);
    }
    Vm2Block *block = dk::handle<Vm2Block>((int16_t)(uint16_t)*vm);
    block->coroutine[0] = **command;
    (*command)++;
    g_WeaponDropper = (uint16_t)*director;
    *d0 = 0;
}
