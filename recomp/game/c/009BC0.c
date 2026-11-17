#include "game.h"

/* One gameplay step ($009BC0-$009BEF): the stage triggers, every object, then the camera; with the
 * debug free camera on (g_FreeCam), the pad moves the camera instead. Two entries differ only in
 * the camera routine they end with. */

/* the triggers and the objects, the registers passing through as the asm leaves them */
static void run_triggers_and_objects(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4,
                                     uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1,
                                     uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    PTR(const TriggerKey) next;
    PTR(const u8) table;
    next.v = *a2;
    table.v = *a3;
    const TriggerKey *n = next;
    const u8 *t = table;
    Trigger_RunX(d0, d1, d2, *d3, d4, d5, d6, d7, a0, a1, &n, &t, a4, a5, a6);
    *a2 = dk::addr(n);
    *a3 = dk::addr(t);
    Obj_RunList(d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, a6);
}

/* Frame_RunWorld - one gameplay step: the stage trigger table (Trigger_RunX), every object
 * (Obj_RunList), then camera tracking (Camera_TrackPlayersX); with g_FreeCam set,
 * Frame_RunFreeCam instead.
 * In:  g_FreeCam; the registers pass through to the calls
 * Out: as the calls hand the registers back
 * Evidence: tst.b g_FreeCam / bne Frame_RunFreeCam / jsr Trigger_RunX / jsr Obj_RunList / jmp
 *           Camera_TrackPlayersX; callers $1FB3A, $3FFBE, $475C2, $47772, $479EE, $47BD0
 */
void Frame_RunWorld(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                    uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
                    uint32_t *a6)
{
    if (g_FreeCam) {
        Frame_RunFreeCam(d0, d1, d2, d3, d4, d5, d6, &d7, a0, a1, a2, *a3, *a4, *a5, a6);
        return;
    }
    run_triggers_and_objects(&d0, d1, d2, d3, d4, d5, d6, &d7, a0, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Camera_TrackPlayersX);
}

/* Frame_RunWorldLayerA - Frame_RunWorld for sections with a scrolling plane A layer: it ends with
 * Camera_TrackPlayersXLayerA, which also stores plane A's offset from the view in g_LayerA.
 * In:  g_FreeCam; the registers pass through to the calls
 * Out: as the calls hand the registers back
 * Evidence: the same tst / bne / jsr Trigger_RunX / jsr Obj_RunList, then jmp
 *           Camera_TrackPlayersXLayerA ($12790); callers $1FC50, $1FD76, $1FECC, $3FE94, $400E4, $403F6
 */
void Frame_RunWorldLayerA(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                          uint32_t *d6, uint32_t d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4,
                          uint32_t *a5, uint32_t *a6)
{
    if (g_FreeCam) {
        Frame_RunFreeCam(d0, d1, d2, d3, d4, d5, d6, &d7, a0, a1, a2, *a3, *a4, *a5, a6);
        return;
    }
    run_triggers_and_objects(&d0, d1, d2, d3, d4, d5, d6, &d7, a0, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Camera_TrackPlayersXLayerA);
}

/* Frame_RunFreeCam - the debug free-camera step taken instead of the objects while g_FreeCam is
 * set: on to Debug_FreeCamStep (the pad moves the camera).
 * In:  the registers Debug_FreeCamStep takes
 * Out: as Debug_FreeCamStep
 * Evidence: jmp Debug_FreeCamStep ($15730); branched to by Frame_RunWorld and Frame_RunWorldLayerA
 *           when g_FreeCam is set (toggled by B in the debug pause menu $1569C)
 */
void Frame_RunFreeCam(uint32_t d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
                      uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t a4, uint32_t a5,
                      uint32_t *a6)
{
    dk::jump(at_Debug_FreeCamStep);
}
