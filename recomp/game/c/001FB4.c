#include "game.h"

/* Op2_BranchTerrainCode, the terrain-routine choice and the set / bit ops on script variable 0
 * (a4 +$22) ($001FB4-$001FF7). */

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_BranchTerrainCode - a2 op: branches when the terrain code byte (the high byte of the
 * terrain routine's d3, stored by the move op) equals the operand's low byte.
 * In:  script = word code, word branch offset; mot->terrain_code
 * Out: code = the operand; the branch (script + offset, NEXT), else Op2_SkipWord
 * Evidence: move.w (a2)+,d0 / cmp.b $50(a3),d0 / bne.w Op2_SkipWord / adda.w (a2),a2 / NEXT; $4CDA
 *           stores d3 at $50 (its meaning per terrain routine is not established; 0 on the flat
 *           floors); 16 corpus runs incl. the TAS
 */
void Op2_BranchTerrainCode(uint16_t *code, uint32_t *handler, const u16 **script, const Vm2Motion *mot)
{
    *code = *(*script)++;
    if ((uint8_t)*code != (uint8_t)(mot->terrain_code >> 8)) {
        dk::jump(at_Op2_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op2_UseMainTerrain - a2 op: the object collides with the scene's main terrain routine
 * g_TerrainFn.
 * In:  -
 * Out: MOT_ALT_TERRAIN cleared; NEXT
 * Evidence: bclr #0,(a3) / NEXT; Coll_MoveScene picks g_TerrainFnAlt when bit 0 is set; 94 corpus
 *           runs incl. the TAS
 */
void Op2_UseMainTerrain(const u16 **script, Vm2Motion *mot)
{
    mot->flags &= (u8)~MOT_ALT_TERRAIN;
    next(script);
}

/* Op2_UseAltTerrain - a2 op: the object collides with the scene's alternative terrain routine
 * g_TerrainFnAlt (e.g. Coll_FlatFloorRest for a flat floor).
 * In:  -
 * Out: MOT_ALT_TERRAIN set; NEXT
 * Evidence: bset #0,(a3) / NEXT; 89 corpus runs incl. the TAS
 */
void Op2_UseAltTerrain(const u16 **script, Vm2Motion *mot)
{
    mot->flags |= MOT_ALT_TERRAIN;
    next(script);
}

/* Op2_ClearVar0 - a2 op: clears script variable 0 (the player code $10322 stores the pad bits
 * there, $11BC4 steps it as an index).
 * In:  -
 * Out: vm->var[0] = 0; NEXT
 * Evidence: clr.w $22(a4) / NEXT; 35 corpus runs incl. the TAS
 */
void Op2_ClearVar0(const u16 **script, Vm2Block *vm)
{
    vm->var[0] = 0;
    next(script);
}

/* Op2_SetVar0 - a2 op: sets script variable 0 to a word.
 * In:  script = word value
 * Out: vm->var[0]; NEXT
 * Evidence: move.w (a2)+,$22(a4) / NEXT; 27 corpus runs incl. the TAS
 */
void Op2_SetVar0(const u16 **script, Vm2Block *vm)
{
    vm->var[0] = *(*script)++;
    next(script);
}

/* Op2_OrVar0 - a2 op: sets bits in script variable 0.
 * In:  script = word bits
 * Out: bits = the operand; vm->var[0] |= bits; NEXT
 * Evidence: move.w (a2)+,d0 / or.w d0,$22(a4) / NEXT; 44 corpus runs incl. the TAS
 */
void Op2_OrVar0(uint16_t *bits, const u16 **script, Vm2Block *vm)
{
    *bits = *(*script)++;
    vm->var[0] = (uint16_t)(vm->var[0] | *bits);
    next(script);
}

/* Op2_AndVar0 - a2 op: masks script variable 0 with a word.
 * In:  script = word mask
 * Out: mask = the operand; vm->var[0] &= mask; NEXT
 * Evidence: move.w (a2)+,d0 / and.w d0,$22(a4) / NEXT; 38 corpus runs incl. the TAS
 */
void Op2_AndVar0(uint16_t *mask, const u16 **script, Vm2Block *vm)
{
    *mask = *(*script)++;
    vm->var[0] = (uint16_t)(vm->var[0] & *mask);
    next(script);
}
