#include "game.h"

/* Span_ClearDrawDisc - clears all 224 span lines, then draws the dithered disc alone
 * (Span_DrawDiscDither, jumped to with the caller's disc arguments).
 * In:  as Span_DrawDiscDither: radius/centre_x/centre_y = d0-d2, h_lines/v_lines = the line
 *      tables (a0/a1); d3-d6/a2/a3 pass on to it
 * Out: as Span_DrawDiscDither; rows (d7) = Span_ClearLines' -6 x 224 going in (DrawDisc's
 *      move.w d0,d7 replaces it)
 * Evidence: movem.l d0-d2/a0-a1,-(a7) / move.w #$E0,d7 / moveq #0,d0 / moveq #0,d2 /
 *           jsr Span_ClearLines / movem.l (a7)+,d0-d2/a0-a1 / jmp Span_DrawDiscDither; jmp from
 *           $334D2; TAS frames 79014-79108
 */
void Span_ClearDrawDisc(uint16_t radius, uint16_t centre_x, uint16_t centre_y, uint16_t *edge, uint16_t *vsum,
                        uint16_t *clear, uint32_t *line, uint16_t *rows, uint32_t h_lines, uint32_t v_lines,
                        uint32_t *top, uint32_t *bottom)
{
    (void)radius; (void)centre_x; (void)centre_y; (void)edge; (void)vsum; (void)clear; (void)line;
    (void)top; (void)bottom;
    /* the movem keeps d0-d2/a0-a1 for the disc: the clear works on copies */
    uint16_t vscroll = 0;
    uint16_t lines = 224;
    uint32_t h = h_lines, v = v_lines;
    Span_ClearLines(0, &vscroll, &lines, &h, &v);
    *rows = lines;
    dk::jump(at_Span_DrawDiscDither);
}
