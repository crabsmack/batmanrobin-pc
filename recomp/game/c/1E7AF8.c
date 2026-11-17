#include "game.h"

enum { Z80_SFX_REQUESTS = 0x17EA };   /* the driver's 6 x 6-byte effect request area (Z80 address) */

/* Snd_SendSfxBody - hands each pending sound-effect slot (data nonzero) to the Z80's request area
 * and clears its data; an idle slot counts its hold down, then drops its priority
 * In:  slots (a0) = g_SndSfxSlots (6 slots)
 * Out: Z80 RAM $17EA + 6n = slot n's 6 bytes (pending slots only); the Z80 bus released
 * Evidence: move.w #$100,$A11100 / btst #0 wait / lea $A017EA,a1 / moveq #5,d7 / tst.l 2(a0) /
 *           pending: 6 x move.b (a0)+,d0 / move.b d0,(a1) / cmp.b (a1),d0 / bne / addq.w #1,a1,
 *           then clr.l -4(a0); idle: tst.b 1(a0) / subq.b #1,1(a0), else clr.b 0(a0) (the
 *           `dc.w $4228,$0000`); each path its own dbra and bus release; entered by Snd_SendSfx
 *           from the V-int every full frame; 146 corpus runs
 */
void Snd_SendSfxBody(SndSfxSlot *slots)
{
    hw::port_write16(hw::Z80_BUSREQ, 0x100);
    while (hw::port_read8(hw::Z80_BUSREQ) & 1) {
    }
    uint32_t request = hw::Z80_RAM + Z80_SFX_REQUESTS;
    for (int i = 0; i < SND_SFX_SLOTS; i++, request += sizeof(SndSfxSlot)) {
        SndSfxSlot *slot = &slots[i];
        if (slot->data != 0u) {
            /* the slot's six bytes as they are, each stored until it reads back */
            const u8 *bytes = (const u8 *)slot;
            for (unsigned b = 0; b < sizeof(SndSfxSlot); b++) {
                do
                    hw::port_write8(request + b, bytes[b]);
                while (hw::port_read8(request + b) != bytes[b]);
            }
            slot->data = 0u;
        } else if (slot->hold != 0) {
            slot->hold = (uint8_t)(slot->hold - 1);
        } else {
            slot->priority = 0;
        }
    }
    hw::port_write16(hw::Z80_BUSREQ, 0);
}
