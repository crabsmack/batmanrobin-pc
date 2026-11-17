#include "game.h"

/* Enemy_Missile_InitFromArgs - spawner tail for the homing missile (body Scr2_0287B0): its
 * heading (target and current) from g_VM2Arg0, the frames it flies straight (var 0) from Arg1
 * and the frames it homes (var 1) from Arg2
 * In:  motion/vm = the new missile's blocks (a3/a4, after Obj_SpawnEnemyOrQuit); the high words
 *      of g_VM2Arg0-2 set by the launcher's GosubArgs op
 * Out: d0 = 0 (moveq: Z set, N/V/C clear)
 * Evidence: move.w g_VM2Arg0,d0 / move.w d0,$A(a3) / move.w d0,$C(a3) / move.w g_VM2Arg1,$22(a4)
 *           / move.w g_VM2Arg2,$24(a4) / moveq #0,d0; jumped to by 11 spawners ($029DCC,
 *           $02CE7E-$02CFCC, $02F514, $02F53C, $03165E-$031720); TAS f43185-f76015. The tag gains
 *           tst(d0.l): the measured ccout NVC is the final moveq's
 */
uint32_t Enemy_Missile_InitFromArgs(Vm2Motion *motion, Vm2Block *vm)
{
    /* move.w from a long: the high word (the big-endian first word) */
    uint16_t heading = (uint16_t)(g_VM2Arg0 >> 16);
    motion->heading_target = heading;
    /* move.w to +$C: the integer part of the 16.16 heading, the fraction kept */
    motion->heading = (s32)(((uint32_t)heading << 16) | ((uint32_t)motion->heading & 0xFFFFu));
    vm->var[0] = (u16)(g_VM2Arg1 >> 16);
    vm->var[1] = (u16)(g_VM2Arg2 >> 16);
    return 0;
}
