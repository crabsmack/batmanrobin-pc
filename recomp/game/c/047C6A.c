#include "game.h"

enum {
    SCROLL_PLANE = 8,          /* g_ScrollMode: whole-plane scroll words */
    PAD_PRESSED = 2,           /* g_PadLive1 +2: the buttons newly pressed */
    BTN_START = 1 << 7,
    RESULT_SKIPPED = 1,        /* g_SceneResult: Start pressed */
};

/* Scene_LoopSkippable - the common loop of the story, attract and stage-opening scenes: whole-
 * plane scroll (mode 8), then Scene_LoopSkippableKeepMode.
 * In:  the registers the scene routines hand on
 * Out: never returns here: the scene ends through its frame callback (Scene_FadeOutExit or a
 *      script's Scene_EndAfterDelay), which unwinds to the scene runner
 * Evidence: move.w #8,g_ScrollMode, falls into $047C70; callers $0484F6, $04885C, $0486B6,
 *           $0487B2, $048AD2, $048CBA, $0494BE, $04A180-$04A464 (bra.w); TAS f10 on
 */
void Scene_LoopSkippable(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                         uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    g_ScrollMode = SCROLL_PLANE;
    Scene_LoopSkippableKeepMode(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, a3, a4, a5, a6);
}

/* Scene_LoopSkippableKeepMode - the scene loop with the Start skip, keeping the scene's own
 * scroll mode: every frame the objects and the camera; once g_SceneSkipDelay has run out (and
 * unless g_IntroNoSkip), Start newly pressed on pad 1 sets g_SceneResult 1 and fades out.
 * In:  the registers the scene routines hand on; g_SceneSkipDelay (frames before Start counts)
 * Out: never returns here (the scene's frame callback unwinds to the scene runner); g_SceneResult
 *      = 1 on Start; g_SceneSkipDelay counted down to 0
 * Evidence: Frame_Begin / Obj_RunList / Camera_UpdateView / tst.b g_IntroNoSkip / subq.w
 *           #1,g_SceneSkipDelay / bpl / clr.w / btst #7,g_PadLive1+2 / move.w #1,g_SceneResult /
 *           Scene_FadeOutExit / Frame_End / bra; the story searchlight scene enters here after
 *           g_ScrollMode = $C
 */
void Scene_LoopSkippableKeepMode(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                 uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                                 uint32_t *a5, uint32_t *a6)
{
    uint32_t a0 = 0;   /* a clobber: the callees set it */
    for (;;) {
        Frame_Begin(&d0, d1, &d7, a1);
        Obj_RunList(&d0, d1, &d2, d3, d4, d5, d6, &d7, &a0, a1, a2, a3, a4, a5, a6);
        Camera_UpdateView(&d0, d1, &d2);
        if (!g_IntroNoSkip) {
            g_SceneSkipDelay -= 1;
            if ((int16_t)(uint16_t)g_SceneSkipDelay < 0) {
                g_SceneSkipDelay = 0;
                if (g_PadLive1[PAD_PRESSED] & BTN_START) {
                    g_SceneResult = RESULT_SKIPPED;
                    dk::Flags ccr{};   /* its condition codes: nothing here reads them */
                    Scene_FadeOutExit(&ccr);
                }
            }
        }
        /* the frame callback that ends the scene runs in here and never comes back */
        Frame_End(&d0, d1, &d2, d3, d4, d5, d6, &d7, a2, a3, a4, *a5, *a6);
    }
}

/* Scene_LoopUnskippable - the scene loop without the Start skip: whole-plane scroll (mode 8),
 * then Scene_LoopUnskippableKeepMode.
 * In:  the registers the scene routines hand on
 * Out: never returns here: the scene's script ends it (Scene_EndAfterDelay etc.)
 * Evidence: move.w #8,g_ScrollMode, falls into $047CB6; callers the ending scenes ($048512,
 *           $04A5AA, $04A626), the continue screen $04916E, the title $0496A2 (bra.w); TAS f392
 */
void Scene_LoopUnskippable(uint32_t d0, uint32_t *d1, uint32_t d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t d7, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                           uint32_t *a5, uint32_t *a6)
{
    g_ScrollMode = SCROLL_PLANE;
    Scene_LoopUnskippableKeepMode(d0, *d1, d2, *d3, *d4, *d5, *d6, d7, *a2, *a3, *a4);
}

/* Scene_LoopUnskippableKeepMode - the scene loop without the Start skip, keeping the scene's own
 * scroll mode: objects and camera every frame until a script ends the scene.
 * In:  the registers the scene routines hand on
 * Out: never returns here (the scene's frame callback unwinds to the scene runner)
 * Evidence: Frame_Begin / Obj_RunList / Camera_UpdateView / Frame_End / bra; the player select
 *           enters here after g_ScrollMode = $30; TAS f412
 */
void Scene_LoopUnskippableKeepMode(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5,
                                   uint32_t d6, uint32_t d7, uint32_t a2, uint32_t a3, uint32_t a4)
{
    uint32_t a0 = 0, a1 = 0, a5 = 0, a6 = 0;   /* not inputs (measured): the callees set them */
    for (;;) {
        Frame_Begin(&d0, &d1, &d7, &a1);
        Obj_RunList(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a0, &a1, &a2, &a3, &a4, &a5, &a6);
        Camera_UpdateView(&d0, &d1, &d2);
        Frame_End(&d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7, &a2, &a3, &a4, a5, a6);
    }
}
