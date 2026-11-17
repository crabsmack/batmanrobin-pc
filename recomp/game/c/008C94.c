#include "game.h"

/* a 68000 address held in a register, as a pointer (PTR(T): the 68000 value kept) */
template <class T> static T *ptr(uint32_t a)
{
    PTR(T) p;
    p.v = a;
    return p;
}

/* The overlap scan of a collision box list ($008C94-$008CB5): from a box on, the first one whose
 * rectangle overlaps d4-d7 (inclusive edges: a box touching the rectangle counts). The list ends
 * at its tail sentinel, the one node whose next link is 0. */

/* the rectangle the scan tests against (d4-d7) */
struct BoxRect_8C94 {
    int16_t left, top, right, bottom;
};

/* the four compares, in the asm's order (each miss branches to the next box) */
static bool overlaps(const HitBox *box, const BoxRect_8C94 &r)
{
    if (r.left > box->right) return false;    /* cmp.w $16(a2),d4 / bgt */
    if (r.right < box->left) return false;    /* cmp.w $12(a2),d6 / blt */
    if (r.top > box->bottom) return false;    /* cmp.w $18(a2),d5 / bgt */
    if (r.bottom < box->top) return false;    /* cmp.w $14(a2),d7 / blt */
    return true;
}

/* the scan from box *a2, tested before it moves on: d0 = -1 and N set (moveq #-1) on a hit; at
   the tail sentinel Z set (tst.w (a2) of its 0 link) */
static void scan(uint32_t *d0, const BoxRect_8C94 &r, uint32_t *a2, bool test_first, dk::Flags *ccr)
{
    const HitBox *box = ptr<const HitBox>(*a2);
    if (!test_first || !overlaps(box, r)) {
        for (;;) {
            *a2 = (uint32_t)(int32_t)box->next;   /* movea.w (a2),a2 */
            box = ptr<const HitBox>(*a2);
            if (box->next == 0) {
                *ccr = dk::Flags::test(0, 2, ccr->x);
                return;
            }
            if (overlaps(box, r))
                break;
        }
    }
    *d0 = 0xFFFFFFFFu;
    *ccr = dk::Flags::test(*d0, 4, ccr->x);
}

/* Hitbox_FindNextBox - continues an overlap scan from the box after a2.
 * In:  a2 = the last box found; d4 left, d5 top, d6 right, d7 bottom
 * Out: Z clear, d0 = -1, a2 = the next overlapping box; Z set at the end of the list (a2 = the
 *      tail sentinel, d0 unchanged)
 * Evidence: movea.w (a2),a2 / tst.w (a2) / beq.s Hitbox_ScanDone / then Hitbox_TestBox's compares;
 *           the loops `jsr (a3) / bsr Hitbox_FindNextBox / bne` at $4E64 and $660A, $3750C
 */
void Hitbox_FindNextBox(uint32_t *d0, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, HitBox **box, dk::Flags *found)
{
    BoxRect_8C94 r = { (int16_t)d4, (int16_t)d5, (int16_t)d6, (int16_t)d7 };
    uint32_t a2 = dk::addr(*box);
    scan(d0, r, &a2, false, found);
    *box = ptr<HitBox>(a2);
}

/* Hitbox_TestBox - the overlap test the box scans start with: box a2 against the rectangle; a miss
 * goes on with the next box (Hitbox_FindNextBox's loop).
 * In:  a2 = a box (not the tail sentinel); d4 left, d5 top, d6 right, d7 bottom
 * Out: Z clear, d0 = -1, a2 = the first overlapping box from a2 on; Z set at the end of the list
 * Evidence: cmp.w $16(a2),d4 / bgt / cmp.w $12(a2),d6 / blt / cmp.w $18(a2),d5 / bgt /
 *           cmp.w $14(a2),d7 / blt / moveq #-1,d0; bne.s here from Hitbox_FindPlayerBox and
 *           Hitbox_FindEnemyBox (the list's first box)
 */
void Hitbox_TestBox(uint32_t *d0, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2, dk::Flags *ccr)
{
    BoxRect_8C94 r = { (int16_t)d4, (int16_t)d5, (int16_t)d6, (int16_t)d7 };
    scan(d0, r, a2, true, ccr);
}

/* Hitbox_ScanDone - the rts the scans end on (a function of its own in the generated code: the
 * end-of-list beq and the hit both reach it).
 * In:  none
 * Out: none (the condition codes stay as the scan left them)
 * Evidence: beq.s Hitbox_ScanDone in Hitbox_FindNextBox; moveq #-1,d0 falls into it
 */
void Hitbox_ScanDone(void)
{
}
