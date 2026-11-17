#include "game.h"

/* Item_LaunchFromHit - shared tail of the FromHit pickup spawners: the new pickup flies off in
 * the hit's direction, its x velocity half the knockback of the box just struck.
 * In:  motion = the new pickup's motion block (a3, left by Obj_SpawnEnemyOrQuit); g_StruckBox
 * Out: motion->vel_x = knockback / 2 (asr: rounds toward minus infinity); 0 (the spawn succeeded)
 * Evidence: movea.l (g_StruckBox).w,a0 / move.l $22(a0),d0 / asr.l #1 / move.l d0,$18(a3) /
 *           moveq #0,d0; jmp'd to by the 11 FromHit spawners; 51 TAS frames
 */
uint32_t Item_LaunchFromHit(Vm2Motion *motion)
{
    PTR(const HitBox) struck;
    struck.v = (uint32_t)g_StruckBox;
    int32_t k = struck->knockback;
    motion->vel_x = k >> 1;   /* arithmetic shift, as asr.l */
    return 0;
}
