#include "game.h"

/* Scene_InitPlain - starts a non-gameplay scene (title, menus, story, stage cards): the common
 * scene reset from the header (Scene_InitCommon), then the fade in (Fade_StartIn).
 * In:  header (a6) = the scene header (Scene_InitCommon's layout)
 * Out: as Scene_InitCommon (the registers it hands back); then Fade_StartIn's fade from black
 * Evidence: bsr.w Scene_InitCommon / bra.w Fade_StartIn; 22 callers, the scene routines
 *           $484F6-$4A626 (attract, title $496A2, player select $49AA6, stage cards $4A180 ...)
 */
void Scene_InitPlain(uint32_t *d0, uint16_t *d1, uint32_t *d2, uint16_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3,
                     uint32_t *a4, uint32_t header)
{
    Scene_InitCommon(d0, d1, d2, d6, d7, a2, a3, a4, &header);
    dk::jump(at_Fade_StartIn);
}
