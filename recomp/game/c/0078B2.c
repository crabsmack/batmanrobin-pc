#include "game.h"

/* one queued DMA command to the VDP control port: VDP regs $93/$94, $95/$96, $97, the command
   (move.l / move.l / move.l / move.w (a3)+,(a0)); the last word written, for the condition codes */
static uint16_t send_entry(uint32_t ctrl, uint32_t *a3)
{
    PTR(const u16) q;
    q.v = *a3;
    for (int i = 0; i < 3; i++) {
        uint32_t hi = (uint16_t)*q++;
        hw::port_write32(ctrl, hi << 16 | (uint16_t)*q++);
    }
    uint16_t last = *q++;
    hw::port_write16(ctrl, last);
    *a3 = q.raw();
    return last;
}

/* DMA_FlushQueue_29 - computed entry into DMA_FlushQueue's unrolled copy: sends 29 queued DMA
 * commands (11 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $78B2 = $785A + 11*8: 29 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_29(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 29; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_16 - computed entry into DMA_FlushQueue's unrolled copy: sends 16 queued DMA
 * commands (24 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $791A = $785A + 24*8: 16 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_16(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 16; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_15 - computed entry into DMA_FlushQueue's unrolled copy: sends 15 queued DMA
 * commands (25 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7922 = $785A + 25*8: 15 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_15(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 15; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_14 - computed entry into DMA_FlushQueue's unrolled copy: sends 14 queued DMA
 * commands (26 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $792A = $785A + 26*8: 14 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_14(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 14; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_13 - computed entry into DMA_FlushQueue's unrolled copy: sends 13 queued DMA
 * commands (27 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7932 = $785A + 27*8: 13 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_13(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 13; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_12 - computed entry into DMA_FlushQueue's unrolled copy: sends 12 queued DMA
 * commands (28 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $793A = $785A + 28*8: 12 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_12(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 12; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_11 - computed entry into DMA_FlushQueue's unrolled copy: sends 11 queued DMA
 * commands (29 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7942 = $785A + 29*8: 11 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_11(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 11; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_10 - computed entry into DMA_FlushQueue's unrolled copy: sends 10 queued DMA
 * commands (30 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $794A = $785A + 30*8: 10 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_10(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 10; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_9 - computed entry into DMA_FlushQueue's unrolled copy: sends 9 queued DMA
 * commands (31 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7952 = $785A + 31*8: 9 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_9(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 9; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_8 - computed entry into DMA_FlushQueue's unrolled copy: sends 8 queued DMA
 * commands (32 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $795A = $785A + 32*8: 8 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_8(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 8; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_7 - computed entry into DMA_FlushQueue's unrolled copy: sends 7 queued DMA
 * commands (33 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7962 = $785A + 33*8: 7 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_7(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 7; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_6 - computed entry into DMA_FlushQueue's unrolled copy: sends 6 queued DMA
 * commands (34 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $796A = $785A + 34*8: 6 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_6(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 6; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_5 - computed entry into DMA_FlushQueue's unrolled copy: sends 5 queued DMA
 * commands (35 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7972 = $785A + 35*8: 5 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_5(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 5; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_4 - computed entry into DMA_FlushQueue's unrolled copy: sends 4 queued DMA
 * commands (36 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $797A = $785A + 36*8: 4 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_4(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 4; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_3 - computed entry into DMA_FlushQueue's unrolled copy: sends 3 queued DMA
 * commands (37 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7982 = $785A + 37*8: 3 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_3(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 3; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_2 - computed entry into DMA_FlushQueue's unrolled copy: sends 2 queued DMA
 * commands (38 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $798A = $785A + 38*8: 2 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_2(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 2; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}

/* DMA_FlushQueue_1 - computed entry into DMA_FlushQueue's unrolled copy: sends 1 queued DMA
 * command (39 slots free).
 * In:  ctrl = the VDP control port (a0), a3 = the queue's start
 * Out: a3 past them; the condition codes of the last move.w
 * Evidence: $7992 = $785A + 39*8: 1 x (move.l / move.l / move.l / move.w (a3)+,(a0)),
 *           then the rts at $799A; reached only by the jmp at $7856
 */
void DMA_FlushQueue_1(uint32_t ctrl, uint32_t *a3, dk::Flags *ccr)
{
    uint16_t last = 0;
    for (int i = 0; i < 1; i++)
        last = send_entry(ctrl, a3);
    *ccr = dk::Flags::test(last, 2, ccr->x);
}
