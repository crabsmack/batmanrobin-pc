#include "game.h"

/* The a6 layer redraw (one block column per frame from Frame_End's hook) and the cell-map ops,
 * which set up a blit (VM6_SetupMapBlit, the next file) and write it now (VM6_BlitMapNow) or
 * through a V-int job (VM6_StartBlitJob). */

/* VM6_StartRedraw - a6: starts a column-per-frame redraw of a layer: saves its block column and
 * plane cell column, takes the column count from the screen width in blocks and installs
 * VM6_RedrawStep as the resume hook.
 * In:  a0 = the layer; a6 = the script pc to resume at
 * Out: g_A6ScriptPc = a6, g_A6RedrawSavedCol / SavedCell = layer block_col / col,
 *      g_A6RedrawCols = screen_blocks_x, g_A6RedrawLayer = the layer, g_A6ResumeHook =
 *      VM6_RedrawStep; the condition codes of that move.l (all clear)
 * Evidence: move.l a6,(g_A6ScriptPc).w / move.w $26(a0) / $32(a0) / $3E(a0) / move.l a0 /
 *           move.l #$743A,(g_A6ResumeHook).w / rts; bra from Op6_RedrawLayerA; 4 corpus runs
 */
void VM6_StartRedraw(uint32_t a0, uint32_t a6, dk::Flags *ccr)
{
    PTR(MapLayer) layer;
    layer.v = a0;
    g_A6ScriptPc = a6;
    g_A6RedrawSavedCol = (uint16_t)layer->block_col;
    g_A6RedrawSavedCell = layer->col;
    g_A6RedrawCols = (uint16_t)layer->screen_blocks_x;
    g_A6RedrawLayer = a0;
    g_A6ResumeHook = at_VM6_RedrawStep;
    *ccr = dk::Flags{ ccr->x, false, false, false, false };
}

/* VM6_RedrawStep - a6 resume hook of the layer redraw: queues the layer's next block column
 * (Layer_QueueColumn, step 1: one block = 2 plane cells); after the last one puts the layer's
 * column counters back and resumes the script from the next frame.
 * In:  g_A6RedrawLayer, g_A6RedrawCols
 * Out: d1 / d3 = 0, d2 = 2 and d4-d7, a2, a3 as Layer_QueueColumn leaves them; a6 = the layer;
 *      when done block_col / col restored and g_A6ResumeHook = VM6_Resume
 * Evidence: movea.l (g_A6RedrawLayer).w,a6 / moveq #1,d0 / #0,d1 / #2,d2 / #0,d3 / jsr
 *           Layer_QueueColumn / subq.w #1,(g_A6RedrawCols).w / bgt / restore +$26 / +$32 /
 *           move.l #$70A8,(g_A6ResumeHook).w; hook value $743A; 4 corpus runs
 */
void VM6_RedrawStep(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6,
                    uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a6)
{
    *a6 = g_A6RedrawLayer;
    *d1 = 0;
    *d2 = 2;
    *d3 = 0;
    uint16_t cell_step = (uint16_t)*d2;
    uint32_t a0 = 0;
    Layer_QueueColumn(1, d1, &cell_step, d3, d4, d5, d6, d7, &a0, a2, a3, *a6);
    *d2 = (*d2 & 0xFFFF0000u) | cell_step;
    g_A6RedrawCols = (uint16_t)(g_A6RedrawCols - 1);
    if ((int16_t)g_A6RedrawCols > 0)
        return;
    PTR(MapLayer) layer;
    layer.v = *a6;
    layer->block_col = (int16_t)g_A6RedrawSavedCol;
    layer->col = g_A6RedrawSavedCell;
    g_A6ResumeHook = at_VM6_Resume;
}

/* the six blit words: src x (bytes), src row, dest x (bytes), dest row, width, height (0 = the
   map's) as d0-d5; movem.w sign-extends them */
static void read_rect(const u16 **script, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                      uint32_t *d5)
{
    uint32_t *r[6] = { d0, d1, d2, d3, d4, d5 };
    for (int i = 0; i < 6; i++)
        *r[i] = (uint32_t)(int32_t)(int16_t)vm6_word(script);
}

/* VM6_SetupMapBlit on d0-d7 (the word-wide ones merged into the longs, upper words kept) */
static void setup_blit(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                       uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6)
{
    uint16_t w2 = (uint16_t)*d2, w4 = (uint16_t)*d4, w5 = (uint16_t)*d5;
    VM6_SetupMapBlit((uint16_t)*d0, d1, &w2, d3, &w4, &w5, d6, d7, a0, a1, a6);
    *d2 = (*d2 & 0xFFFF0000u) | w2;
    *d4 = (*d4 & 0xFFFF0000u) | w4;
    *d5 = (*d5 & 0xFFFF0000u) | w5;
}

/* Op6_DrawMap - a6 op: writes a whole cell map from a buffer entry onto a plane straight to the
 * VDP (a scene setup, the display off), then continues.
 * In:  a6 = script: word g_BufPtrs offset, word entry offset, word address of the plane VRAM
 *      variable, word tile word added to every cell
 * Out: d0-d5 = 0, then as VM6_SetupMapBlit and VM6_BlitMapNow (the next file) leave them
 * Evidence: moveq #0,d0-d5 / bsr VM6_SetupMapBlit / bra VM6_BlitMapNow; 34 uses, e.g. $1FC16
 *           `7468 0000 0000 EE36 02D2` (plane B, tile + $2D2); 24 corpus runs
 */
void Op6_DrawMap(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                 uint32_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a3, uint32_t *a6)
{
    (void)a3;
    *d0 = *d1 = *d2 = *d3 = *d4 = *d5 = 0;
    uint32_t a0 = 0;
    setup_blit(d0, d1, d2, d3, d4, d5, d6, d7, &a0, a1, a6);
    dk::jump(at_VM6_BlitMapNow);
}

/* Op6_DrawMapRect - a6 op: writes a rectangle of a cell map from a buffer entry onto a plane
 * position straight to the VDP, then continues.
 * In:  a6 = script: words src x (bytes), src row, dest x (bytes), dest row, width, height (cells,
 *      0 = the map's), then as Op6_DrawMap
 * Out: as Op6_DrawMap, d0-d5 from the words
 * Evidence: movem.w (a6)+,d0-d5 / bsr VM6_SetupMapBlit / bra VM6_BlitMapNow; 10 uses, e.g. $32E74
 *           `747C 0000 000C 0058 0016 0028 0008 0000 0000 EE34 0396`; 9 corpus runs
 */
void Op6_DrawMapRect(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint16_t *d7, uint32_t *a1, uint32_t *a3, uint32_t *a6)
{
    (void)a3;
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    read_rect(&script, d0, d1, d2, d3, d4, d5);
    *a6 = ADDR68(script);
    uint32_t a0 = 0;
    setup_blit(d0, d1, d2, d3, d4, d5, d6, d7, &a0, a1, a6);
    dk::jump(at_VM6_BlitMapNow);
}

/* Op6_DrawMapVInt - a6 op: writes a whole cell map from a buffer entry onto a plane through a
 * V-int job (rows as the DMA budget allows), parking the script until it is done.
 * In:  a6 = script: word g_BufPtrs offset, word entry offset, word address of the plane VRAM
 *      variable, word tile word
 * Out: d0-d5 = 0, then as VM6_SetupMapBlit and VM6_StartBlitJob leave them
 * Evidence: moveq #0,d0-d5 / bsr VM6_SetupMapBlit / bra VM6_StartBlitJob; 2 uses: $33114
 *           `7488 0004 0000 EE36 455A`, $49D9A `7488 0000 0000 EE36 02B6`; 8 corpus runs
 */
void Op6_DrawMapVInt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6)
{
    *d0 = *d1 = *d2 = *d3 = *d4 = *d5 = 0;
    setup_blit(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a6);
    dk::jump(at_VM6_StartBlitJob);
}

/* Op6_DrawMapRectVInt - a6 op: writes a rectangle of a cell map from a buffer entry onto a plane
 * position through a V-int job, parking the script until it is done.
 * In:  a6 = script: words src x (bytes), src row, dest x (bytes), dest row, width, height, then as
 *      Op6_DrawMapVInt
 * Out: as Op6_DrawMapVInt, d0-d5 from the words
 * Evidence: movem.w (a6)+,d0-d5 / bsr VM6_SetupMapBlit / bra VM6_StartBlitJob; 6 corpus runs
 */
void Op6_DrawMapRectVInt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                         uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    read_rect(&script, d0, d1, d2, d3, d4, d5);
    *a6 = ADDR68(script);
    setup_blit(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a6);
    dk::jump(at_VM6_StartBlitJob);
}
