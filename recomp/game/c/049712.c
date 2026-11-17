#include "game.h"

/* the title menu script, entered again after the options page (the bytes after the jsr $7FE.w) */
static constexpr uint32_t Scr2_049720 = 0x049720u;

enum { SONG_TITLE = 0x11 };

/* Title_BackFromOptions - native run by Start on the options page: restarts the title song (the
 * music test may have changed it), then re-enters the title menu script.
 * In:  the running director's registers
 * Out: Sound_PlayMusicAnyway's song $11 and its d0 / d1 / d7, which the script continues with at
 *      Scr2_049720 (jsr $7FE.w)
 * Evidence: move.w #$11,d0 / jsr Sound_PlayMusicAnyway / jsr $7FE.w; reached by Op2_BranchVar0Any
 *           $80 at $49764 -> Op2_Native $49710; ran only in scenario-options (f1449)
 */
void Title_BackFromOptions(uint32_t *d0, uint32_t *d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                           uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    *d0 = (*d0 & 0xFFFF0000u) | SONG_TITLE;   /* move.w: the upper word stays */
    Sound_PlayMusicAnyway(d0, d1, d7, a1, a2, a3, a4, a5, a6);
    dk::jump_script(at_VM2_Enter, Scr2_049720);
}
