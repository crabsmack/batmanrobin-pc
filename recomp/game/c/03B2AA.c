#include "game.h"

/* the head's frame loop after the native's jsr $7FE.w (VM2_Enter): step the coroutines, run the
   three children, yield */
static constexpr uint32_t Scr2_03B2BA = 0x03B2BAu;

/* Boss_CatHead_PublishPos - native fragment of the cat head's frame loop: publishes the head's
 * position, which Boss_CatHead_FrameScroll scrolls the face picture (plane A) to follow
 * In:  head = the head object (a6)
 * Out: the arena's head_x/head_y (g_SceneVars+8/+$A); then the script at Scr2_03B2BA
 * Evidence: move.w $3E(a6),(g_SceneVars+8).w / move.w $42(a6),(g_SceneVars+$A).w / jsr ($7FE).w;
 *           after Op2_PlaceBodyBoxes / Tint_StepLine3 at $03B2A8; TAS f87285-f90392 (2670
 *           frames); RAM f88320: head (160,99) = g_SceneVars+8/+$A $A0/$63
 */
void Boss_CatHead_PublishPos(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                             uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5,
                             const Vm2Obj *head)
{
    CatHeadArena *arena = cat_head_arena();
    arena->head_x = head->x;
    arena->head_y = head->y;
    dk::jump_script(at_VM2_Enter, Scr2_03B2BA);
}
