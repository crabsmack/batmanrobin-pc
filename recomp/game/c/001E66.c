#include "game.h"

/* The terrain box B ops ($001E66-$001E9D). Box B (a3 +$48-$4E, on while MOT_BOX_B) is collided
 * first by the move op $4CDA, which falls back to box A only when B found no floor. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(const u16 **script)
{
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetTerrainBoxB - a2 op: sets and turns on terrain box B with the contact results and the
 * terrain code cleared.
 * In:  script = word dx, word dy, word wall mask, word floor mask
 * Out: mot->wall_hit / contact / terrain_code = 0; then as Op2_SetTerrainBoxBKeepResults
 * Evidence: clr.w $52/$54/$50(a3), then the moves to $48-$4E / bset #2,(a3); 8 corpus runs
 *           incl. the TAS
 */
void Op2_SetTerrainBoxB(const u16 **script, Vm2Motion *mot)
{
    mot->wall_hit = 0;
    mot->contact = 0;
    mot->terrain_code = 0;
    Op2_SetTerrainBoxBKeepResults(script, mot);
}

/* Op2_SetTerrainBoxBKeepResults - a2 op: sets terrain box B's offset and masks and turns it on,
 * the contact results kept (Op2_SetTerrainBoxB past its clears).
 * In:  script = word dx, word dy, word wall mask, word floor mask
 * Out: box B; then as Op2_EnableTerrainBoxB
 * Evidence: move.w (a2)+ to $48/$4A/$4C/$4E(a3); a computed-jump target in the corpus (the NEXT
 *           of Op2_SetTerrainBoxATouching, 507 call-site entries)
 */
void Op2_SetTerrainBoxBKeepResults(const u16 **script, Vm2Motion *mot)
{
    mot->box_b_dx = (int16_t)*(*script)++;
    mot->box_b_dy = (int16_t)*(*script)++;
    mot->box_b_wall_mask = *(*script)++;
    mot->box_b_floor_mask = *(*script)++;
    Op2_EnableTerrainBoxB(script, mot);
}

/* Op2_EnableTerrainBoxB - a2 op: turns terrain box B on with its offset, masks and contact
 * results as they are.
 * In:  -
 * Out: MOT_BOX_B set; NEXT
 * Evidence: bset #2,(a3) / NEXT; a computed-jump target in the corpus (~20k call-site entries)
 */
void Op2_EnableTerrainBoxB(const u16 **script, Vm2Motion *mot)
{
    mot->flags |= MOT_BOX_B;
    next(script);
}

/* Op2_DisableTerrainBoxB - a2 op: turns terrain box B off.
 * In:  -
 * Out: MOT_BOX_B cleared; NEXT
 * Evidence: bclr #2,(a3) / NEXT; $4CDA tests bit 2 of (a3); 89 corpus runs incl. the TAS
 */
void Op2_DisableTerrainBoxB(const u16 **script, Vm2Motion *mot)
{
    mot->flags &= (u8)~MOT_BOX_B;
    next(script);
}

/* Op2_SetTerrainBoxBMasks - a2 op: changes only terrain box B's wall and floor masks.
 * In:  script = word wall mask, word floor mask
 * Out: mot->box_b_wall_mask / box_b_floor_mask; NEXT
 * Evidence: move.w (a2)+,$4C(a3) / move.w (a2)+,$4E(a3) / NEXT; 5 fuzz corpus runs (not the TAS)
 */
void Op2_SetTerrainBoxBMasks(const u16 **script, Vm2Motion *mot)
{
    mot->box_b_wall_mask = *(*script)++;
    mot->box_b_floor_mask = *(*script)++;
    next(script);
}
