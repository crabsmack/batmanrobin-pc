#include "game.h"

/* ArtCache_Alloc ($0085F8-$0086A9).
 *
 * The original keeps the cache's ring pointers in d2-d7 for the whole routine (movem.w
 * (g_ArtRelRingRd).w,d2-d7: sign-extended, so each is a $FFFF8xxx address; each ring steps by
 * addq.b #2, wrapping in its 256 bytes) and stores them back at the end. The C does the same
 * with named locals that are those registers (the prototype hands them back). */

/* a ring pointer register's next slot: addq.b #2 (the low byte only) */
static void ring_step(uint32_t *r) { *r = (*r & ~0xFFu) | (uint8_t)(*r + 2); }

/* the word at a 68000 address held in a register */
static u16 *word_at(uint32_t a)
{
    PTR(u16) p;
    p.v = a;
    return p;
}

static uint32_t sext16(uint16_t w) { return (uint32_t)(int32_t)(int16_t)w; }

/* the descriptor a ring slot holds */
static ArtDesc *slot_desc(uint32_t slot) { return dk::handle<ArtDesc>((int16_t)(uint16_t)*word_at(slot)); }

/* the word the original reads at a descriptor's +0 (refcount byte : block count byte) */
static uint16_t desc_word(const ArtDesc *d) { return (uint16_t)(((uint8_t)d->refcount << 8) | d->blocks); }

/* ArtCache_Alloc - gives an art handle a descriptor and `blocks` VRAM blocks, taking blocks from
 * the free ring while it has them and otherwise evicting released art from the release ring
 * (oldest first): an evicted descriptor's handle is cleared, its blocks are taken over and any it
 * has beyond the request go back to the free ring. With no spare descriptor, the oldest released
 * one is reused in place (its own blocks first). Released descriptors that are in use again
 * (refcount < 0) are only taken off the ring (ring_slot = 0).
 * In:  handle = the art handle's address (a0); blocks (d1.b; the caller has already reserved
 *      g_ArtBlocksFree / g_ArtDescsFree)
 * Out: the handle = the descriptor (refcount -1, blocks, ring_slot 0, owner = handle, tiles);
 *      list = its block list (a3); the ring pointers stored back; d2-d7 = those registers
 *      (g_ArtRelRingRd, DescRingWr, DescRingRd, BlockRingWr, BlockRingRd and the
 *      g_ArtBlocksFree:Spare word), blocks = the dbra / subq.b counter as left, a1 = the last
 *      evicted owner or ring slot written (unchanged when none); d0/a0/a2 clobbered
 * Evidence: movem.w (g_ArtRelRingRd).w,d2-d7 / subq.b #1,(g_ArtDescsSpare).w / bcs, cmp.b d7,d1 /
 *           bgt (signed), the movea.l d6,a0 / move.w (a0),(a2)+ dbra, the eviction loop's
 *           move.w (a0),d0 / ble, dbeq d0 / bne, add.b d0,d7, the surplus to movea.l d5,a1, the
 *           movem.w d2-d7 store; callers Spr_SwapFrameArt, ArtCache_Acquire, $6776
 */
void ArtCache_Alloc(uint16_t *blocks, uint32_t *rel_rd, uint32_t *desc_wr, uint32_t *desc_rd, uint32_t *blk_wr,
                    uint32_t *blk_rd, uint32_t *spare, uint32_t handle, uint32_t *a1, uint32_t *list)
{
    *rel_rd = sext16(g_ArtRelRingRd);
    *desc_wr = sext16(g_ArtDescRingWr);
    *desc_rd = sext16(g_ArtDescRingRd);
    *blk_wr = sext16(g_ArtBlockRingWr);
    *blk_rd = sext16(g_ArtBlockRingRd);
    *spare = sext16((uint16_t)((g_ArtBlocksFree << 8) | g_ArtBlocksSpare));   /* the low byte: Spare */

    uint16_t d0 = 0;                /* the low word of d0: a descriptor's word, then its block counter */
    ArtDesc *desc;
    const u16 *src = nullptr;       /* a0: the tiles of the descriptor being taken over */
    bool take_over;                 /* enter at the copy of a descriptor's own blocks ($864E) */

    if (g_ArtDescsSpare-- == 0) {   /* no spare descriptor: reuse the oldest released one in place */
        g_ArtDescsSpare = 0;
        for (;;) {
            desc = slot_desc(sext16((uint16_t)*rel_rd));
            ring_step(rel_rd);
            d0 = desc_word(desc);
            if (!(d0 & 0x8000u))
                break;
            desc->ring_slot = 0;    /* in use again: only off the ring */
        }
        *word_at(handle) = dk::handle_of(desc);
        desc->refcount = (int8_t)(desc->refcount - 1);
        desc->blocks = (uint8_t)*blocks;
        desc->ring_slot = 0;
        *a1 = sext16(desc->owner);
        *word_at(*a1) = 0;          /* the old handle loses its art */
        desc->owner = (uint16_t)handle;
        src = desc->tiles;
        take_over = true;
    } else {
        desc = slot_desc(*desc_rd);
        ring_step(desc_rd);
        *word_at(handle) = dk::handle_of(desc);
        desc->refcount = (int8_t)(desc->refcount - 1);
        desc->blocks = (uint8_t)*blocks;
        desc->ring_slot = 0;
        desc->owner = (uint16_t)handle;
        take_over = false;
    }
    u16 *dst = desc->tiles;         /* a2 */
    *list = ADDR68(dst);

    for (;;) {
        if (!take_over) {
            if ((int8_t)*blocks <= (int8_t)*spare) {
                /* enough blocks in the free ring: take the rest from it */
                *spare = (*spare & ~0xFFu) | (uint8_t)(*spare - (uint8_t)*blocks);
                *blocks = (uint16_t)((*blocks & 0xFF00u) | (uint8_t)(*blocks - 1));
                do {
                    *dst++ = *word_at(*blk_rd);
                    ring_step(blk_rd);
                } while ((*blocks = (uint16_t)(*blocks - 1)) != 0xFFFF);
                break;
            }
            /* evict the oldest released descriptor (skipping those in use again) */
            for (;;) {
                desc = slot_desc(*rel_rd);
                ring_step(rel_rd);
                d0 = desc_word(desc);
                if ((int16_t)d0 > 0)
                    break;
                desc->ring_slot = 0;
            }
            g_ArtDescsSpare = (uint8_t)(g_ArtDescsSpare + 1);
            *word_at(*desc_wr) = dk::handle_of(desc);   /* back on the free-descriptor ring */
            ring_step(desc_wr);
            *a1 = sext16(desc->owner);
            *word_at(*a1) = 0;
            src = desc->tiles;
        }
        take_over = false;
        /* take over its blocks until the request is met ($864E: dbeq) */
        d0 = (uint16_t)(d0 - 1);
        bool met;
        for (;;) {
            *dst++ = *src++;
            *blocks = (uint16_t)((*blocks & 0xFF00u) | (uint8_t)(*blocks - 1));
            met = (uint8_t)*blocks == 0;
            if (met)
                break;
            if ((d0 = (uint16_t)(d0 - 1)) == 0xFFFF)
                break;
        }
        if (!met)
            continue;               /* bne: still short, this descriptor used up */
        /* its blocks beyond the request go back to the free ring */
        *spare = (*spare & ~0xFFu) | (uint8_t)(*spare + (uint8_t)d0);
        while ((d0 = (uint16_t)(d0 - 1)) != 0xFFFF) {
            *a1 = *blk_wr;
            ring_step(blk_wr);
            *word_at(*a1) = *src++;
        }
        break;
    }

    g_ArtRelRingRd = (uint16_t)*rel_rd;
    g_ArtDescRingWr = (uint16_t)*desc_wr;
    g_ArtDescRingRd = (uint16_t)*desc_rd;
    g_ArtBlockRingWr = (uint16_t)*blk_wr;
    g_ArtBlockRingRd = (uint16_t)*blk_rd;
    g_ArtBlocksFree = (uint8_t)(*spare >> 8);
    g_ArtBlocksSpare = (uint8_t)*spare;
}
