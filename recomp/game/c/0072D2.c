#include "game.h"

/* The a6 plane-fill V-int job ($0072D2-$007365): start it, write rows in the V-int as the DMA
 * budget allows, idle when done, and free it from Frame_End's hook. V-int code: verify skips the
 * calls an interrupt lands in, so D2 proves VM6_VIntClearRows. */

/* VM6_StartClearJob - a6: starts the plane-fill V-int job: allocates a V-int list object
 * (retrying from the hook each frame while the pool is full) with handler VM6_VIntClearRows, and
 * parks the script.
 * In:  the fill set up by VM6_SetupClearRect (g_A6TileWord, g_A6VdpCmd, g_A6RectCols/Rows)
 * Out: d0 = 0, a6 = the object (job), g_A6VIntJob = it, g_A6ResumeHook = 0; or d0 = -1 and
 *      g_A6ResumeHook = VM6_StartClearJob (try again next frame); the condition codes of the last
 *      move.l (all clear) either way
 * Evidence: jsr VIntObj_NewIdle / beq / move.l #$72D2,(g_A6ResumeHook).w / rts / clr.l
 *           (g_A6ResumeHook).w / move.w a6,(g_A6VIntJob).w / move.l #$7302,(a6) / rts; 2 corpus
 *           runs (TAS f32080, f77148)
 */
void VM6_StartClearJob(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a6, dk::Flags *ccr)
{
    VIntObj_NewIdle(d0, a0, a1, a6);
    *ccr = dk::Flags{ ccr->x, false, false, false, false };
    if (*d0 != 0) {
        g_A6ResumeHook = at_VM6_StartClearJob;
        return;
    }
    g_A6ResumeHook = 0;
    g_A6VIntJob = (uint16_t)*a6;
    PTR(ObjNode) job;
    job.v = *a6;
    job->handler = at_VM6_VIntClearRows;
}

/* VM6_EndClearJob - a6 resume hook after a plane fill: frees the V-int job object and resumes
 * the script.
 * In:  g_A6VIntJob = the job
 * Out: a6 = the job, a1 as VIntObj_UnlinkFree leaves it; VM6_Resume
 * Evidence: movea.w (g_A6VIntJob).w,a6 / jsr VIntObj_UnlinkFree / bra VM6_Resume; hook value $72F4
 *           set by VM6_VIntClearRows after the last row; 2 corpus runs
 */
void VM6_EndClearJob(uint32_t *a1, uint32_t *a6)
{
    *a6 = (uint32_t)(int32_t)(int16_t)g_A6VIntJob;
    *a1 = VIntObj_UnlinkFree(*a6);
    dk::jump(at_VM6_Resume);
}

/* VM6_VIntClearRows - V-int list handler of the plane-fill job: writes as many rows of the fill
 * tile straight to the VDP as the DMA budget allows (budget * 1.5 cells); after the last row it
 * idles and leaves the end to Frame_End.
 * In:  job = the object (a6); ctrl / data = the VDP ports (a0 / a1, the V-int's); g_A6TileWord,
 *      g_A6VdpCmd, g_A6RectCols, g_A6RectRows
 * Out: g_A6VdpCmd at the next row; when the last is written: the handler = VM6_ClearJobIdle,
 *      g_A6ResumeHook = VM6_EndClearJob. Nothing while g_FrameBusy (the V-int's busy path)
 * Evidence: tst.b (g_FrameBusy).w / bne / budget + budget asr 1 / divu.w cols+1 / subq.w #1 / bmi /
 *           move.l d1,(a0) / move.w d0,(a1) dbra / subq.w #1,(g_A6RectRows).w / dbmi d7 / bmi ->
 *           move.l #$7364,(a6) / move.l #$72F4,(g_A6ResumeHook).w; run from VIntObj_RunList in 2
 *           corpus runs
 */
void VM6_VIntClearRows(uint32_t ctrl, uint32_t data, uint32_t job)
{
    if (g_FrameBusy)
        return;
    uint16_t budget = g_DMABudgetLeft;
    uint16_t cells = (uint16_t)(budget + ((int16_t)budget >> 1));   /* add.w d6,d7 */
    uint16_t rows = (uint16_t)((uint32_t)cells / (uint16_t)(g_A6RectCols + 1));   /* divu.w */
    rows = (uint16_t)(rows - 1);
    if ((int16_t)rows < 0)
        return;
    uint16_t tile = g_A6TileWord;
    uint32_t cmd = g_A6VdpCmd;
    uint32_t row_step = (uint32_t)(uint16_t)g_PlaneRowBytes << 2;
    bool last;
    for (;;) {
        hw::port_write32(ctrl, cmd);
        uint16_t n = g_A6RectCols;
        do
            hw::port_write16(data, tile);
        while (n-- != 0);
        cmd = vm6_next_row_cmd(cmd, row_step);
        g_A6RectRows = (uint16_t)(g_A6RectRows - 1);
        last = (int16_t)g_A6RectRows < 0;
        if (last)
            break;   /* dbmi: minus ends the loop without counting */
        if (rows-- == 0)
            break;
    }
    if (!last) {
        g_A6VdpCmd = cmd;
        return;
    }
    PTR(ObjNode) self;
    self.v = job;
    self->handler = at_VM6_ClearJobIdle;
    g_A6ResumeHook = at_VM6_EndClearJob;
}

/* VM6_ClearJobIdle - V-int list handler of a finished plane-fill job: does nothing until Frame_End's
 * VM6_EndClearJob frees the object.
 * In:  the job (a6)
 * Out: -
 * Evidence: the rts at $7364, stored as the handler by VM6_VIntClearRows after the last row (and
 *           its early exits); 2 corpus runs
 */
void VM6_ClearJobIdle(void)
{
}
