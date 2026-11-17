#include "game.h"

/* Mot_JumpLaunchSpeed and the terrain box A ops ($001DBC-$001E2F). Terrain box A (a3 +$40-$46,
 * on while MOT_BOX_A) is the box the move op $4CDA collides through Coll_MoveScene; its wall and
 * floor results land in +$52 / +$54. */

/* divs.w: the 32-bit dividend by a signed word; the remainder in the high word, the quotient in
   the low. A quotient that doesn't fit a word sets V and leaves the dividend unchanged. */
static uint32_t divs_w(uint32_t dividend, int16_t divisor)
{
    if (divisor == 0)
        dk::fatal("Mot_JumpLaunchSpeed: divs.w by zero (a speed factor of 0)");
    int64_t q = (int64_t)(int32_t)dividend / divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    int32_t r = (int32_t)dividend % divisor;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Mot_JumpLaunchSpeed - the launch speed of a jump: -(gravity >> 4) * the runs to the peak, the
 * runs being the time word just read (script[-1]) * 256 / the speed factor.
 * In:  speed = the gravity (from Mot_JumpGravity); script = just past the time word; vm->speed
 * Out: runs = divs.w(time << 8, vm->speed) (remainder:quotient); speed = the launch velocity
 *      (negative = up for a positive height)
 * Evidence: moveq #0,d0 / move.w -2(a2),d0 / asl.l #8,d0 / divs.w $2E(a4),d0 / asr.l #4,d1 /
 *           muls.w d0,d1 / neg.l d1 / rts; callers Op2_ArcX, Op2_JumpAnim, Op2_Jump, Op2_FallAnim;
 *           executed in 137 corpus runs
 */
void Mot_JumpLaunchSpeed(uint32_t *runs, uint32_t *speed, const u16 *script, const Vm2Block *vm)
{
    *runs = divs_w((uint32_t)script[-1] << 8, (int16_t)vm->speed);
    int16_t g = (int16_t)(uint16_t)((int32_t)*speed >> 4);
    *speed = 0u - (uint32_t)((int32_t)g * (int16_t)(uint16_t)*runs);
}

/* Op2_SetTerrainBoxATouching - a2 op: sets and turns on terrain box A, with the contact results
 * preset to the masks (it starts as touching) and the terrain code cleared.
 * In:  script = word dx, word dy, word wall mask, word floor mask
 * Out: mask = the floor mask; mot->terrain_code = 0, box A, wall_hit / contact = the masks,
 *      MOT_BOX_A; NEXT
 * Evidence: clr.w $50(a3) / move.w (a2)+ to $40, $42, d0 -> $44 and $52, d0 -> $46 and $54 /
 *           bset #1,(a3) / NEXT; 38 corpus runs incl. the TAS
 */
void Op2_SetTerrainBoxATouching(uint16_t *mask, const u16 **script, Vm2Motion *mot)
{
    mot->terrain_code = 0;
    mot->box_a_dx = (int16_t)*(*script)++;
    mot->box_a_dy = (int16_t)*(*script)++;
    *mask = *(*script)++;
    mot->box_a_wall_mask = *mask;
    mot->wall_hit = *mask;
    *mask = *(*script)++;
    mot->box_a_floor_mask = *mask;
    mot->contact = *mask;
    mot->flags |= MOT_BOX_A;
    next(script);
}

/* Op2_SetTerrainBoxA - a2 op: sets and turns on terrain box A with the contact results and the
 * terrain code cleared.
 * In:  script = word dx, word dy, word wall mask, word floor mask
 * Out: mot->wall_hit / contact / terrain_code = 0, box A; then as Op2_EnableTerrainBoxA
 * Evidence: clr.w $52/$54/$50(a3) / move.w (a2)+ to $40-$46, then Op2_EnableTerrainBoxA's
 *           bset; 65 corpus runs incl. the TAS
 */
void Op2_SetTerrainBoxA(const u16 **script, Vm2Motion *mot)
{
    mot->wall_hit = 0;
    mot->contact = 0;
    mot->terrain_code = 0;
    mot->box_a_dx = (int16_t)*(*script)++;
    mot->box_a_dy = (int16_t)*(*script)++;
    mot->box_a_wall_mask = *(*script)++;
    mot->box_a_floor_mask = *(*script)++;
    Op2_EnableTerrainBoxA(script, mot);
}

/* Op2_EnableTerrainBoxA - a2 op: turns terrain box A on with its offset, masks and contact
 * results as they are (Op2_SetTerrainBoxA past its setup).
 * In:  -
 * Out: MOT_BOX_A set; NEXT
 * Evidence: bset #1,(a3) / NEXT; a computed-jump target in the corpus (940 call-site entries)
 */
void Op2_EnableTerrainBoxA(const u16 **script, Vm2Motion *mot)
{
    mot->flags |= MOT_BOX_A;
    next(script);
}

/* Op2_DisableTerrainBoxA - a2 op: turns terrain box A off, so the move op no longer collides it.
 * In:  -
 * Out: MOT_BOX_A cleared; NEXT
 * Evidence: bclr #1,(a3) / NEXT; $4CDA tests bit 1 of (a3); 35 corpus runs incl. the TAS
 */
void Op2_DisableTerrainBoxA(const u16 **script, Vm2Motion *mot)
{
    mot->flags &= (u8)~MOT_BOX_A;
    next(script);
}

/* Op2_SetTerrainBoxAMasks - a2 op: changes only terrain box A's wall and floor masks (they go to
 * Coll_MoveScene in d6/d7).
 * In:  script = word wall mask, word floor mask
 * Out: mot->box_a_wall_mask / box_a_floor_mask; NEXT
 * Evidence: move.w (a2)+,$44(a3) / move.w (a2)+,$46(a3) / NEXT; 89 corpus runs incl. the TAS
 */
void Op2_SetTerrainBoxAMasks(const u16 **script, Vm2Motion *mot)
{
    mot->box_a_wall_mask = *(*script)++;
    mot->box_a_floor_mask = *(*script)++;
    next(script);
}
