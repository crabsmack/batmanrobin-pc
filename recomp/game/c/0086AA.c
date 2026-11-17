#include "game.h"

/* ArtCache_Release ($0086AA-$0086E5). */

/* ArtCache_Release - drops one reference to the art behind a handle; at the last one its blocks
 * count as free again and the descriptor is queued on the release ring (once: a queued one keeps
 * its slot), where ArtCache_Alloc may evict it later.
 * In:  handle = the art handle's address (a0)
 * Out: result (d0) = 0 released, 1 = still referenced, 2 = was not in use (a refcount >= 0: reset
 *      to 0), -1 = the handle is empty; d1.b = the block count when released (d1.w = the ring slot
 *      when it was queued); a0 clobbered
 * Evidence: move.w (a0),d0 / beq, addq.b #1,(a0)+ / bne / bmi, move.b (a0)+,d1 /
 *           add.b d1,(g_ArtBlocksFree).w / tst.w (a0) / bne, move.w (g_ArtRelRingWr).w,d1 /
 *           addq.b #2,(g_ArtRelRingWrLo).w / move.w d1,(a0) / movea.w d1,a0 / move.w d0,(a0),
 *           addq.b #1,(g_ArtDescsFree).w; callers Spr_InitBankFrames, $11D4E
 */
void ArtCache_Release(uint32_t *result, uint16_t *d1, uint32_t handle)
{
    PTR(u16) h;
    h.v = handle;
    uint16_t desc_addr = *(u16 *)h;
    if (desc_addr == 0) {
        *result = 0xFFFFFFFFu;
        return;
    }
    ArtDesc *desc = dk::handle<ArtDesc>((int16_t)desc_addr);
    int8_t users = (int8_t)(desc->refcount + 1);
    desc->refcount = users;
    if (users != 0) {
        if (users < 0) {
            *result = 1;
        } else {
            desc->refcount = 0;
            *result = 2;
        }
        return;
    }
    *d1 = (uint16_t)((*d1 & 0xFF00u) | desc->blocks);
    g_ArtBlocksFree = (uint8_t)(g_ArtBlocksFree + desc->blocks);
    if (desc->ring_slot == 0) {
        *d1 = g_ArtRelRingWr;
        g_ArtRelRingWrLo = (uint8_t)(g_ArtRelRingWrLo + 2);
        desc->ring_slot = *d1;
        *dk::handle<u16>((int16_t)*d1) = desc_addr;
    }
    g_ArtDescsFree = (uint8_t)(g_ArtDescsFree + 1);
    *result = 0;
}
