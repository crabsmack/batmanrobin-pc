#include "game.h"

/* the movem.l d0-d7/a0-a6,-(a7) / (a7)+ around the handler: the same 15 longs on the stack */
static void save_registers(const uint32_t r[15])
{
    for (int i = 14; i >= 0; i--)
        dk::push_long(r[i]);
}
static void restore_registers(uint32_t r[15])
{
    for (int i = 0; i < 15; i++)
        r[i] = dk::pop_long();
}

/* take the Z80's bus (request it with the Z80 out of reset) and wait for the grant */
static void z80_take_bus(void)
{
    hw::port_write16(hw::Z80_BUSREQ, 0x100);
    hw::port_write16(hw::Z80_RESET, 0x100);
    while (hw::port_read8(hw::Z80_BUSREQ) & 1) {
    }
}

/* VInt_Handler - the vertical-blank interrupt: display off, then (unless the main loop is still
 * building a frame) flushes the DMA queue, runs the V-int objects, reads the pads, uploads the
 * palette and scroll, then with interrupts on the sound and the fade; signals the frame done. When
 * the background decompressor was running, it parks it: its SR / PC and registers go to
 * g_BgTaskContext and the RTE returns to whoever resumed it.
 * In:  g_VIntOff (set: a bare RTE), g_FrameBusy (set: VInt_BusyPath), g_BgTaskRunning; the
 *      interrupted registers
 * Out: g_VIntDone = $FF, g_VIntCount + 1; the registers as interrupted, except when parking: d0's
 *      low word = the task's SR, a4 = its PC (both in the context too); the RTE's SR
 * Evidence: tst.b (g_VIntOff).w / bne rte / movem.l d0-d7/a0-a6,-(a7) / lea $C00004,a0 / -4(a0),a1
 *           / lea g_VDPCmdBuf,a2 / move.w #$8134,(a0) / #$8F02 / tst.b (g_FrameBusy).w / bne
 *           VInt_BusyPath / bsr DMA_FlushQueue / jsr VIntObj_RunList / the Z80 bus around
 *           Pad_ReadAll / Pal_VIntUpload / VInt_UploadScroll / addq.b #1,(g_FrameBusy).w /
 *           andi.w #$F8FF,sr / the bus around Snd_SendSfx / g_FadeLevelVInt / Fade_Step / st.b
 *           (g_VIntDone).w / addq.l #1,(g_VIntCount).w / movem.l (a7)+ / tst.b (g_BgTaskRunning).w /
 *           move.w (a7)+,d0 / movea.l (a7)+,a4 / movem.l d0-d2/d6-d7/a0-a4,(g_BgTaskContext).w /
 *           move.w d0,-(a7) / subq.b #1,(g_FrameBusy).w / rte; vector 30, every TAS frame from 10.
 *           Interrupt code: D2 proves it (ruling 6: vectors are D2 / D6's)
 */
void VInt_Handler(uint32_t *d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6,
                  uint32_t d7, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t *a4, uint32_t a5,
                  uint32_t a6)
{
    if (g_VIntOff) {
        dk::rte();
        return;
    }
    uint32_t saved[15] = { *d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, *a4, a5, a6 };
    save_registers(saved);
    uint32_t ctrl = hw::VDP_CTRL, data = hw::VDP_DATA, cmd_buf = ADDR68(&g_VDPCmdBuf);
    hw::port_write16(ctrl, 0x8134);   /* reg 1: display off */
    hw::port_write16(ctrl, 0x8F02);   /* reg 15: auto-increment 2 */
    if (g_FrameBusy) {
        /* VInt_BusyPath: the main loop is still building the frame: objects, the busy scroll and
           the pads only */
        VIntObj_RunList(d1, d2, d3, d4, d5, d6, d7, &ctrl, &data, &cmd_buf);
        VInt_BusyScroll(ctrl);
        z80_take_bus();
        uint32_t port = *a4, rec = a5;
        Pad_ReadLive(&port, &rec);
        hw::port_write16(hw::Z80_BUSREQ, 0);
        g_VIntCount = g_VIntCount + 1u;
        restore_registers(saved);
        dk::rte();
        return;
    }
    DMA_FlushQueue(ctrl);
    VIntObj_RunList(d1, d2, d3, d4, d5, d6, d7, &ctrl, &data, &cmd_buf);
    z80_take_bus();
    Pad_ReadAll();
    hw::port_write16(hw::Z80_BUSREQ, 0);
    Pal_VIntUpload(ctrl, data, cmd_buf);
    VInt_UploadScroll(ctrl, data, cmd_buf);
    g_FrameBusy = (uint8_t)(g_FrameBusy + 1);   /* a V-int from here on takes the busy path */
    dk::set_interrupt_mask(0);
    z80_take_bus();
    Snd_SendSfx();
    hw::port_write16(hw::Z80_BUSREQ, 0);
    g_FadeLevelVInt = g_FadeLevel;
    Fade_Step();
    g_VIntDone = 0xFF;
    g_VIntCount = g_VIntCount + 1u;
    restore_registers(saved);
    if (g_BgTaskRunning) {
        /* park the background task: its exception frame's SR and PC, its registers, then an RTE
           with the SR alone, onto the address below (the call that resumed it) */
        g_BgTaskRunning = 0;
        PTR(const u16) sr_at;
        sr_at.v = dk::stack_pointer();
        uint16_t sr = *sr_at;
        PTR(const u32) pc_at;
        pc_at.v = sr_at.raw() + 2;
        uint32_t pc = *pc_at;
        dk::set_stack_pointer(sr_at.raw() + 6);
        *d0 = (saved[0] & 0xFFFF0000u) | sr;
        *a4 = pc;
        BgTaskContext *ctx = (BgTaskContext *)g_BgTaskContext;
        ctx->d0 = *d0;
        ctx->d1 = saved[1];
        ctx->d2 = saved[2];
        ctx->d6 = saved[6];
        ctx->d7 = saved[7];
        ctx->a0 = saved[8];
        ctx->a1 = saved[9];
        ctx->a2 = saved[10];
        ctx->a3 = saved[11];
        ctx->a4 = *a4;
        dk::push_word(sr);
    }
    g_FrameBusy = (uint8_t)(g_FrameBusy - 1);
    dk::rte();
}
