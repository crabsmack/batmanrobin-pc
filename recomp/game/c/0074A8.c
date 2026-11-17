#include "game.h"

/* The a6 cell-map blit (set up, written now or by a V-int job), the VRAM DMA ops (now, or through
 * the frame's DMA queue a budget's worth per frame) and Section_SetIdInline ($0074A8-$007733).
 * The V-int handler VM6_VIntBlitRows is proven by D2 (verify skips the calls an interrupt lands
 * in). */

/* VDP register writes of a 68000-to-VRAM DMA (the asm's arithmetic, whole registers): source
   (regs $95-$97: the address / 2, `lsr.l #1 / andi.l #$7F00FF / ori.l #$97009500`, then `lsr.w #8
   / ori.w #$9600`) and length in words (regs $93/$94 from a byte count: `lsl.l #7 / lsr.w #8 /
   andi.l #$FF00FF / ori.l #$94009300`) */
static uint32_t dma_src_regs(uint32_t src, uint16_t *reg96)
{
    src >>= 1;
    uint16_t mid = (uint16_t)src;
    *reg96 = (uint16_t)((mid >> 8) | 0x9600);
    return (src & 0x7F00FFu) | 0x97009500u;
}
static uint32_t dma_len_regs(uint32_t bytes)
{
    bytes <<= 7;
    bytes = (bytes & 0xFFFF0000u) | (uint16_t)((uint16_t)bytes >> 8);
    return (bytes & 0xFF00FFu) | 0x94009300u;
}
/* the VRAM-write command with the DMA bit (`lsl.l #2 / lsr.w #2 / swap / ori.l #$40000080` on a
   word address) */
static uint32_t dma_vram_cmd(uint32_t dest)
{
    dest <<= 2;
    dest = (dest & 0xFFFF0000u) | (uint16_t)((uint16_t)dest >> 2);
    dest = dest << 16 | dest >> 16;
    return dest | 0x40000080u;
}

/* VM6_SetupMapBlit - a6 helper: sets up a cell-map blit from a buffer entry (word width, word
 * height, then the cell words): the source row pointer and stride, the tile word, the VDP write
 * command of the destination and the column / row counts, clipped to the plane.
 * In:  d0 = src x (bytes), d1 = src row, d2 = dest x (bytes), d3 = dest row, d4 / d5 = width /
 *      height (0 = the map's); a6 = script: word g_BufPtrs offset, word entry offset, word address
 *      of the plane VRAM variable (added to d2), word tile word
 * Out: g_A6MapStride = d6 = width * 2; g_A6MapRowPtr = a0 = map + d0 + d1 * stride (d1 = that
 *      product); g_A6TileWord; g_A6VdpCmd = d3; g_A6RectCols / Rows = d4 / d5 (count - 1); d2 =
 *      dest x + plane base; d7 = the last clip difference; a1 = the plane variable's address;
 *      a6 past the operands = g_A6ScriptPc
 * Evidence: the buffer entry / movea.w (a6)+,a1 / add.w (a1),d2 / move.w (a6)+,(g_A6TileWord).w /
 *           moveq #0,d6 / move.w (a0)+,d6 / move.w (a0)+,d7 / the two clips (sub.w / bpl / add.w)
 *           / mulu.w d6,d1 / mulu.w (g_PlaneRowBytes).w,d3 / the command; callers Op6_DrawMap and
 *           its three variants; e.g. `7468 0000 0000 EE36 02D2`; 34 corpus runs
 */
void VM6_SetupMapBlit(uint16_t d0, uint32_t *d1, uint16_t *d2, uint32_t *d3, uint16_t *d4, uint16_t *d5,
                      uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    uint32_t buf = vm6_slot(vm6_word(&script))->raw();
    PTR(const u16) map;
    map.v = vm6_entry(buf, vm6_word(&script));
    *a1 = (uint32_t)(int32_t)(int16_t)vm6_word(&script);
    PTR(const u16) plane_var;
    plane_var.v = *a1;
    *d2 = (uint16_t)(*d2 + *plane_var);
    g_A6TileWord = vm6_word(&script);
    *d6 = (uint16_t)*map++;   /* the map's width and height in cells */
    *d7 = (uint16_t)*map++;
    if (*d4 == 0)
        *d4 = (uint16_t)*d6;
    if (*d5 == 0)
        *d5 = *d7;
    *d6 = (uint16_t)(*d6 + *d6);
    g_A6MapStride = *d6;
    /* clip to the plane: a negative room left shrinks the count */
    *d7 = (uint16_t)(g_PlaneCellsX - *d4 - *d2);
    if ((int16_t)*d7 < 0)
        *d4 = (uint16_t)(*d4 + *d7);
    *d7 = (uint16_t)(g_PlaneCellsY - *d5 - (uint16_t)*d3);
    if ((int16_t)*d7 < 0)
        *d5 = (uint16_t)(*d5 + *d7);
    *d4 = (uint16_t)(*d4 - 1);
    g_A6RectCols = *d4;
    *d5 = (uint16_t)(*d5 - 1);
    g_A6RectRows = *d5;
    uint32_t row_ptr = map.raw() + (uint32_t)(int32_t)(int16_t)d0;
    *d1 = (uint32_t)(uint16_t)*d1 * (uint16_t)*d6;
    row_ptr += *d1;
    *a0 = row_ptr;
    g_A6MapRowPtr = row_ptr;
    uint32_t at = (uint32_t)(uint16_t)*d3 * (uint16_t)g_PlaneRowBytes;
    at = (at & 0xFFFF0000u) | (uint16_t)(at + *d2);
    *d3 = vm6_vram_write_cmd(at);
    g_A6VdpCmd = *d3;
    *a6 = ADDR68(script);
    g_A6ScriptPc = *a6;
}

/* VM6_BlitMapNow - a6: writes the set-up map rectangle straight to the VDP data port, row by row
 * (each cell plus the tile word), then continues the script.
 * In:  g_A6MapRowPtr, g_A6MapStride, g_A6TileWord, g_A6VdpCmd, g_A6RectCols / Rows
 * Out: g_A6MapRowPtr past the rows; d0 = the tile word, d1 = the command after the last row, d2 =
 *      the row step, d3 = the last cell written, d6 = the stride, d7 = $FFFF, a1 = the data port,
 *      a3 past the last row's cells; then VM6_Reload
 * Evidence: lea ($C00004).l,a0 / lea -4(a0),a1 / move.l d1,(a0) / move.w (a3)+,d3 / add.w d0,d3 /
 *           move.w d3,(a1) dbra d6 / add.l d6,(g_A6MapRowPtr).w / the next-row command / dbra d7 /
 *           bra VM6_Reload; bra from Op6_DrawMap / DrawMapRect (scene setup, display off); 24
 *           corpus runs
 */
void VM6_BlitMapNow(uint16_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint32_t *d6, uint16_t *d7,
                    uint32_t *a1, uint32_t *a3, uint32_t *a6)
{
    (void)a6;
    *a1 = hw::VDP_DATA;
    *d0 = g_A6TileWord;
    *d1 = g_A6VdpCmd;
    *d2 = (uint32_t)(uint16_t)g_PlaneRowBytes << 2;
    *d7 = g_A6RectRows;
    do {
        hw::port_write32(hw::VDP_CTRL, *d1);
        PTR(const u16) cells;
        cells.v = g_A6MapRowPtr;
        uint16_t n = g_A6RectCols;
        do {
            *d3 = (uint16_t)(*cells++ + *d0);
            hw::port_write16(*a1, *d3);
        } while (n-- != 0);
        *a3 = cells.raw();
        *d6 = g_A6MapStride;
        g_A6MapRowPtr = g_A6MapRowPtr + *d6;
        *d1 = vm6_next_row_cmd(*d1, *d2);
    } while ((*d7)-- != 0);
    dk::jump(at_VM6_Reload);
}

/* VM6_StartBlitJob - a6: starts the map-blit V-int job: allocates a V-int list object (retrying
 * from the hook each frame while the pool is full) with handler VM6_VIntBlitRows, and parks the
 * script.
 * In:  the blit set up by VM6_SetupMapBlit
 * Out: d0 = 0, a6 = the object, g_A6VIntJob = it, g_A6ResumeHook = 0; or d0 = -1 and
 *      g_A6ResumeHook = VM6_StartBlitJob; the condition codes of the last move.l (all clear)
 * Evidence: jsr VIntObj_NewIdle / beq / move.l #$756E,(g_A6ResumeHook).w / rts / clr.l
 *           (g_A6ResumeHook).w / move.w a6,(g_A6VIntJob).w / move.l #$759E,(a6) / rts; bra from
 *           Op6_DrawMapVInt / DrawMapRectVInt; 9 corpus runs
 */
void VM6_StartBlitJob(uint32_t *d0, uint32_t *a0, uint32_t *a1, uint32_t *a6, dk::Flags *ccr)
{
    VIntObj_NewIdle(d0, a0, a1, a6);
    *ccr = dk::Flags{ ccr->x, false, false, false, false };
    if (*d0 != 0) {
        g_A6ResumeHook = at_VM6_StartBlitJob;
        return;
    }
    g_A6ResumeHook = 0;
    g_A6VIntJob = (uint16_t)*a6;
    PTR(ObjNode) job;
    job.v = *a6;
    job->handler = at_VM6_VIntBlitRows;
}

/* VM6_EndBlitJob - a6 resume hook after a map blit: frees the V-int job object and resumes the
 * script.
 * In:  g_A6VIntJob = the job
 * Out: a6 = the job, a1 as VIntObj_UnlinkFree leaves it; VM6_Resume
 * Evidence: movea.w (g_A6VIntJob).w,a6 / jsr VIntObj_UnlinkFree / bra VM6_Resume; hook value $7590
 *           set by VM6_VIntBlitRows after the last row; 9 corpus runs
 */
void VM6_EndBlitJob(uint32_t *a1, uint32_t *a6)
{
    *a6 = (uint32_t)(int32_t)(int16_t)g_A6VIntJob;
    *a1 = VIntObj_UnlinkFree(*a6);
    dk::jump(at_VM6_Resume);
}

/* VM6_VIntBlitRows - V-int list handler of the map-blit job: writes map rows to the VDP while the
 * DMA budget (in cells, the row's width each) lasts; after the last row it idles and leaves the
 * end to Frame_End.
 * In:  job = the object (a6); ctrl / data = the VDP ports (a0 / a1, the V-int's); the blit state
 *      g_A6*
 * Out: g_A6MapRowPtr past the rows written; g_A6VdpCmd at the next row when the budget ran out;
 *      after the last row the handler = VM6_BlitJobIdle and g_A6ResumeHook = VM6_EndBlitJob.
 *      Nothing while g_FrameBusy
 * Evidence: tst.b (g_FrameBusy).w / bne / move.w (g_DMABudgetLeft).w,d7 / sub.w d6,d7 / ble (store
 *           the command, rts) / the row / subq.w #1,(g_A6RectRows).w / bpl / move.l #$75FC,(a6) /
 *           move.l #$7590,(g_A6ResumeHook).w; run from VIntObj_RunList in 9 corpus runs
 */
void VM6_VIntBlitRows(uint32_t ctrl, uint32_t data, uint32_t job)
{
    if (g_FrameBusy)
        return;
    uint16_t tile = g_A6TileWord;
    uint32_t cmd = g_A6VdpCmd;
    uint32_t row_step = (uint32_t)(uint16_t)g_PlaneRowBytes << 2;
    int16_t budget = (int16_t)g_DMABudgetLeft;
    int16_t width = (int16_t)(g_A6RectCols + 1);
    for (;;) {
        bool out = budget <= width;   /* sub.w d6,d7 / ble: the signed compare, overflow included */
        budget = (int16_t)(budget - width);
        if (out) {
            g_A6VdpCmd = cmd;
            return;
        }
        hw::port_write32(ctrl, cmd);
        PTR(const u16) cells;
        cells.v = g_A6MapRowPtr;
        uint16_t n = g_A6RectCols;
        do
            hw::port_write16(data, (uint16_t)(*cells++ + tile));
        while (n-- != 0);
        g_A6MapRowPtr = g_A6MapRowPtr + g_A6MapStride;
        cmd = vm6_next_row_cmd(cmd, row_step);
        g_A6RectRows = (uint16_t)(g_A6RectRows - 1);
        if ((int16_t)g_A6RectRows < 0)
            break;
    }
    PTR(ObjNode) self;
    self.v = job;
    self->handler = at_VM6_BlitJobIdle;
    g_A6ResumeHook = at_VM6_EndBlitJob;
}

/* VM6_BlitJobIdle - V-int list handler of a finished map-blit job: does nothing until Frame_End's
 * VM6_EndBlitJob frees the object.
 * In:  the job (a6)
 * Out: -
 * Evidence: the rts at $75FC, stored as the handler by VM6_VIntBlitRows after the last row (and
 *           its g_FrameBusy exit); 9 corpus runs
 */
void VM6_BlitJobIdle(void)
{
}

/* Op6_DMAEntry - a6 op: DMAs a buffer entry (word byte length, then the data) to VRAM at once,
 * the display blanked during the transfer unless g_FadeLevel >= $E, then continues.
 * In:  a6 = script: word g_BufPtrs offset, word entry offset, word VRAM address
 * Out: VDP regs 1 ($8134 / $8174), $95-$97, $93-$94, the command (through g_VDPCmdBuf), reg 1 =
 *      $64 again; d0 = the command, d1 = the length registers, d2 = the source registers, d3's
 *      low word = reg $96's write, d7's = reg 1's; VM6_Next
 * Evidence: the buffer entry / move.w (a6)+,d0 / move.w (a0)+,d1 / move.l a0,d2 / move.w #$8134,d7 /
 *           cmpi.b #$E,(g_FadeLevel).w / blt / #$8174 / the register arithmetic / move.l
 *           d0,(g_VDPCmdBuf).w / two move.w to the port / move.w #$8164,(a0) / bra VM6_Next; 46
 *           uses, e.g. Tbl_01FAB2 `7604 0000 0004 4940`; 24 corpus runs
 */
void Op6_DMAEntry(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint16_t *d3, uint16_t *d7, uint32_t *a6)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    uint32_t buf = vm6_slot(vm6_word(&script))->raw();
    PTR(const u16) entry;
    entry.v = vm6_entry(buf, vm6_word(&script));
    *d0 = (*d0 & 0xFFFF0000u) | vm6_word(&script);
    *a6 = ADDR68(script);
    *d1 = (*d1 & 0xFFFF0000u) | (uint16_t)*entry++;   /* the length in bytes */
    *d2 = entry.raw();
    *d7 = (int8_t)g_FadeLevel < 0xE ? 0x8134 : 0x8174;   /* reg 1: display off unless faded in */
    hw::port_write16(hw::VDP_CTRL, *d7);
    *d2 = dma_src_regs(*d2, d3);
    hw::port_write32(hw::VDP_CTRL, *d2);
    hw::port_write16(hw::VDP_CTRL, *d3);
    *d1 = dma_len_regs(*d1);
    hw::port_write32(hw::VDP_CTRL, *d1);
    *d0 = dma_vram_cmd(*d0 & 0xFFFFu);
    g_VDPCmdBuf = *d0;
    hw::port_write16(hw::VDP_CTRL, (uint16_t)(g_VDPCmdBuf >> 16));   /* the second word starts the DMA */
    hw::port_write16(hw::VDP_CTRL, (uint16_t)g_VDPCmdBuf);
    hw::port_write16(hw::VDP_CTRL, 0x8164);
    dk::jump(at_VM6_Next);
}

/* Op6_QueueDMAEntry - a6 op: sends a buffer entry (word byte length, then tiles) to VRAM through
 * the frame's DMA queue, as many tiles per frame as the DMA budget allows (VM6_DMAStep), parking
 * the script until all are sent.
 * In:  a6 = script: word g_BufPtrs offset, word entry offset, word VRAM address
 * Out: g_A6DmaDest, g_A6DmaTiles = d0 = length / 32, g_A6DmaSrc = a0 = the tiles, g_A6ScriptPc
 *      after the operands, g_A6ResumeHook = VM6_DMAStep; the condition codes of that move.l (all
 *      clear)
 * Evidence: the buffer entry / move.w (a6)+,(g_A6DmaDest).w / move.w (a0)+,d0 / lsr.w #5,d0 /
 *           move.l a0,(g_A6DmaSrc).w / move.l a6,(g_A6ScriptPc).w / move.l #$76AE,(g_A6ResumeHook).w
 *           / rts; 8 uses, e.g. $33060 `7682 0000 0000 6340`; 8 corpus runs
 */
void Op6_QueueDMAEntry(uint16_t *d0, uint32_t *a0, uint32_t *a6, dk::Flags *ccr)
{
    PTR(const u16) pc;
    pc.v = *a6;
    const u16 *script = pc;
    uint32_t buf = vm6_slot(vm6_word(&script))->raw();
    PTR(const u16) entry;
    entry.v = vm6_entry(buf, vm6_word(&script));
    g_A6DmaDest = vm6_word(&script);
    *d0 = (uint16_t)((uint16_t)*entry++ >> 5);   /* bytes / 32 = tiles */
    g_A6DmaTiles = *d0;
    *a0 = entry.raw();
    g_A6DmaSrc = *a0;
    *a6 = ADDR68(script);
    g_A6ScriptPc = *a6;
    g_A6ResumeHook = at_VM6_DMAStep;
    *ccr = dk::Flags{ ccr->x, false, false, false, false };
}

/* VM6_DMAStep - a6 resume hook of Op6_QueueDMAEntry: queues one DMA of as many tiles as this
 * frame's DMA budget leaves (keeping 5 units), advancing the source and destination; resumes the
 * script once no tile is left.
 * In:  g_A6DmaSrc, g_A6DmaDest, g_A6DmaTiles
 * Out: one DMA queue entry (14 bytes at g_DMAQueuePtr, which moves past it; g_DMABudgetLeft = what
 *      is left), or nothing when the budget or the queue is short; d1-d3 the asm's (d2 = tiles
 *      sent x 32 << 7 as the length registers, d3 = the command); VM6_Resume when done
 * Evidence: moveq #0,d2 / move.w (g_A6DmaTiles).w,d2 / beq VM6_Resume / budget - 5 / ble / sub.w
 *           d2,d3 / bpl / add.w d3,d2 / moveq #0,d3 / moveq #1,d0 / jsr DMA_ReserveSlots / bne /
 *           the entry / move.w a0,(g_DMAQueuePtr).w; hook value $76AE; 8 corpus runs
 */
void VM6_DMAStep(uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t a0, uint32_t *a6)
{
    (void)a0;
    *d2 = g_A6DmaTiles;
    if (*d2 == 0) {
        dk::jump(at_VM6_Resume);
        return;
    }
    uint16_t left = g_DMABudgetLeft;
    bool none = (int16_t)left <= 5;   /* subq.w #5 / ble: the signed compare */
    left = (uint16_t)(left - 5);
    *d3 = (*d3 & 0xFFFF0000u) | left;
    if (none)
        return;
    left = (uint16_t)(left - (uint16_t)*d2);
    *d3 = (*d3 & 0xFFFF0000u) | left;
    if ((int16_t)left < 0) {   /* fewer than all: send what the budget covers */
        *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)(*d2 + left);
        *d3 = 0;
    }
    uint32_t slots = 1;
    uint32_t at = 0;
    DMA_ReserveSlots(&slots, &at);
    if (slots != 0)
        return;
    g_DMABudgetLeft = (uint16_t)*d3;
    g_A6DmaTiles = (uint16_t)(g_A6DmaTiles - (uint16_t)*d2);
    *d2 = (*d2 & 0xFFFF0000u) | (uint16_t)((uint16_t)*d2 << 5);   /* tiles -> bytes */
    uint32_t src = g_A6DmaSrc;
    *d3 = g_A6DmaDest;
    g_A6DmaSrc = g_A6DmaSrc + *d2;
    g_A6DmaDest = (uint16_t)(g_A6DmaDest + *d2);
    PTR(u16) q;
    q.v = at;
    uint16_t reg96;
    uint32_t src_regs = dma_src_regs(src, &reg96);
    *d1 = reg96;
    *q++ = (uint16_t)(src_regs >> 16);
    *q++ = (uint16_t)src_regs;
    *q++ = reg96;
    *d2 = dma_len_regs(*d2);
    *q++ = (uint16_t)(*d2 >> 16);
    *q++ = (uint16_t)*d2;
    *d3 = dma_vram_cmd(*d3);
    *q++ = (uint16_t)(*d3 >> 16);
    *q++ = (uint16_t)*d3;
    g_DMAQueuePtr = (uint16_t)q.raw();
}

/* Section_SetIdInline - native an a6 script calls (Op6_CallNative): sets g_SectionId to the inline
 * word after the routine's long.
 * In:  a6 -> the section id word
 * Out: a6 past it; g_SectionId
 * Evidence: move.w (a6)+,(g_SectionId).w / rts; dc.l $772E in 7 a6 scripts, e.g. $3301E
 *           `70E0 0000 772E 2200`, $33050 `... 2300` (the stage 2 sections, Tbl_0330FC); 4 corpus
 *           runs
 */
void Section_SetIdInline(const u16 **script)
{
    g_SectionId = vm6_word(script);
}
