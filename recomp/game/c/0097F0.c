#include "game.h"

/* Scroll_RunParallax - runs a parallax command list (Scroll_NextCmd) with the V-int's scroll upload
 * set to per-line (g_ScrollMode $C: the line buffers g_HScrollLinesA/B DMA'd to VRAM $BC00/$BC02).
 * In:  list = the command list (a5); the registers the commands use pass through
 * Out: g_ScrollMode = $C; then as Scroll_NextCmd
 * Evidence: move.w #$C,(g_ScrollMode).w, then the fall into Scroll_NextCmd; callers the stage frame
 *           loops ($1FCDA, $1FDEA, $3FF1E, $4016E, $40454, $49612: lea Tbl_x,a5 / jsr $97F0)
 */
void Scroll_RunParallax(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d6, uint16_t *d7, uint32_t *bands,
                        const u16 **list, uint32_t *layer)
{
    g_ScrollMode = 0xC;
    dk::jump(at_Scroll_NextCmd);
}
