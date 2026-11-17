#include "game.h"

/* the section routines by id: (word id, long routine) entries, the table after Scene_RunList */
static constexpr uint32_t Tbl_00A0AC = 0x00A0ACu;

/* Section_RunNext - finds the section that follows g_SectionId in an order table, makes it current
 * and runs its routine (from Tbl_00A0AC); no entry for the current section: the game is complete.
 * In:  order (a0) = (from, to) word pairs, a long 0 ends; g_SectionId
 * Out: g_SectionId = the next section; ccr Z clear (d0 = -1) after its routine returned; Z set
 *      (d0 = 0) when g_SectionId had no entry; the registers as the routine hands them back
 * Evidence: move.w g_SectionId,d1 / move.l (a0)+,d0 / beq / swap / cmp.w d1,d0 / bne; swap /
 *           move.w d0,g_SectionId / lea Tbl_00A0AC,a0 / cmp.w (a0)+,d0 / beq / addq.l #4,a0;
 *           movea.l (a0),a0 / jsr (a0) / moveq #-1,d0; caller Main_RunSection. The search of
 *           Tbl_00A0AC has no end test: every section id in the order tables is in it.
 */
void Section_RunNext(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, const u32 *order,
                     uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    uint16_t current = g_SectionId;
    *d1 = (*d1 & 0xFFFF0000u) | current;
    uint32_t pair;
    do {
        pair = *order++;
        if (pair == 0) {
            *ccr = dk::Flags::test(0, 4, ccr->x);     /* move.l (a0)+,d0 / beq: Z set */
            return;
        }
    } while ((uint16_t)(pair >> 16) != current);
    uint16_t next = (uint16_t)pair;
    g_SectionId = next;
    PTR(const u16) table;
    table.v = Tbl_00A0AC;
    const u16 *entry = table;
    while (*entry++ != next)
        entry += 2;                                  /* the routine long of another id */
    uint32_t routine = ((uint32_t)entry[0] << 16) | entry[1];
    SceneFn(routine, d1, d3, d4, d5, d6, a1, a2, a3, a4, a5, a6);
    *ccr = dk::Flags::test(0xFFFFFFFFu, 4, ccr->x); /* moveq #-1,d0: Z clear */
}
