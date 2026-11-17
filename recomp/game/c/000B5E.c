#include "game.h"

/* The palette-load ops and Op2_SetDepthFromY ($000B5E-$000BCF). */

/* a long operand (two script words, high first): movea.l (a2)+ */
static uint32_t long_operand(const u16 **script)
{
    uint32_t hi = *(*script)++;
    return hi << 16 | *(*script)++;
}

/* Op2_LoadPalette1 - a2 op: copies the 16-colour palette at the long operand into palette line 1
 * of g_PalSource (Pal_LoadLine1, which flags it for the fade code); NEXT.
 * In:  script = long palette address
 * Out: a1 as Pal_LoadLine1 leaves it; NEXT
 * Evidence: movea.l (a2)+,a0 / jsr Pal_LoadLine1 / NEXT; ran once in the TAS (frame 100733)
 */
void Op2_LoadPalette1(uint32_t *a1, const u16 **script)
{
    uint32_t src = long_operand(script);
    dk::Flags cc{};   /* its condition codes stay in the CPU for the next op */
    Pal_LoadLine1(&src, a1, &cc);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_LoadPalette2 - a2 op: copies the palette at the long operand into line 2 (Pal_LoadLine2);
 * NEXT.
 * In:  script = long palette address
 * Out: src (a0) = the handler (NEXT), a1 as Pal_LoadLine2 leaves it; its condition codes
 * Evidence: movea.l (a2)+,a0 / jsr Pal_LoadLine2 / NEXT; executed in the TAS
 */
void Op2_LoadPalette2(uint32_t *src, uint32_t *a1, const u16 **script)
{
    *src = long_operand(script);
    dk::Flags cc{};   /* its condition codes stay in the CPU for the next op */
    Pal_LoadLine2(src, a1, &cc);
    *src = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*src);
}

/* Op2_LoadPalette3 - a2 op: copies the palette at the long operand into line 3 (Pal_LoadLine3);
 * NEXT.
 * In:  script = long palette address
 * Out: a1 as Pal_LoadLine3 leaves it; NEXT
 * Evidence: movea.l (a2)+,a0 / jsr Pal_LoadLine3 / NEXT; executed in the TAS
 */
void Op2_LoadPalette3(uint32_t *a1, const u16 **script)
{
    uint32_t src = long_operand(script);
    dk::Flags cc{};   /* its condition codes stay in the CPU for the next op */
    Pal_LoadLine3(&src, a1, &cc);
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}

/* Op2_SetDepthFromY - a2 op: the sort depth from the screen y, so lower objects draw in front:
 * (y - camera y) * 32, clamped to [$800, $2000] (a negative product gives $800, so the clamp
 * applies to the 16-bit wrap the asl leaves), plus the operand.
 * In:  script = word bias; self->y, g_CameraY
 * Out: depth = spr->depth; NEXT
 * Evidence: move.w $42(a6),d0 / sub.w (g_CameraY).w,d0 / asl.w #5,d0 / bpl / move.w #$800 /
 *           cmpi.w #$2000 / ble / move.w #$2000 / add.w (a2)+,d0 / move.w d0,$3A(a5); executed in the TAS
 */
void Op2_SetDepthFromY(uint16_t *depth, const u16 **script, Vm2Sprite *spr, const Vm2Obj *self)
{
    int16_t d = (int16_t)(uint16_t)((uint16_t)(self->y - (int16_t)(g_CameraY >> 16)) << 5);
    if (d < 0)
        d = 0x800;
    if (d > 0x2000)
        d = 0x2000;
    *depth = (uint16_t)(d + *(*script)++);
    spr->depth = *depth;
    dk::jump(dk::code((int16_t)(uint16_t)*(*script)++));
}
