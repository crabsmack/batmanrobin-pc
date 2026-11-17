#include "game.h"

/* The tile-queue writer ($012D06-$012DC1) and, falling after it in the same function file,
 * Trigger_LoadX ($012DC2-$012E01).
 *
 * The original writes the queue with an unrolled run of 44 four-byte units (`move.w (a3)+,(a0)` /
 * `move.l (a3)+,(a1)`: one VDP command word, then two tiles to the data port), entered by
 * `jmp loc_012DB6(pc,d0.w)` at the run's end minus the entry's count word. The C loops over the
 * same writes, in the same order. */

/* units in the unrolled run $012D06-$012DB5 */
static constexpr int RUN_UNITS = (0x012DB6 - 0x012D06) / 4;

/* An entry's count word as writes: the jump's displacement back into the run, 4 bytes a unit. */
static int writes_of(int16_t displacement)
{
    if (displacement > 0 || (-displacement) % 4 != 0 || -displacement / 4 > RUN_UNITS)
        dk::fatal("tile queue: count word $%04X lands outside the unrolled writer", (unsigned)(uint16_t)displacement);
    return -displacement / 4;
}

/* `n` units of the run: each a command word to the control port, then a long to the data port */
static void write_cells(uint32_t ctrl, uint32_t data, const u16 **queue, int n)
{
    const TileQueueCell *c = (const TileQueueCell *)*queue;
    for (int i = 0; i < n; i++, c++) {
        hw::port_write16(ctrl, c->command);
        hw::port_write32(data, c->tiles);
    }
    *queue = (const u16 *)c;
}

/* the run's tail ($012DB6) and Layer_WriteTilesStart, entry after entry: the count word (0 ends
 * the queue), the command's high word, then its cells */
static void write_entries(uint32_t ctrl, uint32_t data, const u16 **queue)
{
    for (;;) {
        int16_t displacement = (int16_t)(uint16_t)*(*queue)++;
        if (displacement == 0)
            return;
        hw::port_write16(ctrl, *(*queue)++);
        write_cells(ctrl, data, queue, writes_of(displacement));
    }
}

/* Layer_WriteTiles - the unrolled tile-queue writer entered at its first unit: all 44 cell writes,
 * then the queue's next entries until a 0 count.
 * In:  ctrl/data = the VDP control and data ports (a0/a1); queue = the first cell (a3)
 * Out: queue = past the 0 count that ended the queue
 * Evidence: 22 x (move.w (a3)+,(a0) / move.l (a3)+,(a1)) twice, then move.w (a3)+,d0 / beq.s
 *           Layer_WriteTilesEnd and the fall into Layer_WriteTilesStart; only ever entered by the
 *           computed jump at $012DBC (dkit.toml computed_entry_runs), so its start is that jump's
 *           largest count
 */
void Layer_WriteTiles(uint32_t ctrl, uint32_t data, const u16 **queue)
{
    write_cells(ctrl, data, queue, RUN_UNITS);
    write_entries(ctrl, data, queue);
}

/* Layer_WriteTilesStart - one tile-queue entry: the VDP command's high word to the control port,
 * then the entry's cells (the jump into the run by its count), then the entries after it.
 * In:  displacement = the entry's count word, -(4 x cells) (d0); ctrl/data = the VDP ports
 *      (a0/a1); queue = the entry's command word (a3)
 * Out: queue = past the 0 count that ended the queue
 * Evidence: move.w (a3)+,(a0) / jmp loc_012DB6(pc,d0.w); Layer_FlushTileQueue's bra target and the
 *           run's loop; the queue entries Layer_QueueColumn writes (g_TileQueue)
 */
void Layer_WriteTilesStart(int16_t displacement, uint32_t ctrl, uint32_t data, const u16 **queue)
{
    hw::port_write16(ctrl, *(*queue)++);
    write_cells(ctrl, data, queue, writes_of(displacement));
    write_entries(ctrl, data, queue);
}

/* Layer_WriteTilesEnd - the rts that ends the tile-queue writer (a 0 count; Layer_FlushTileQueue's
 * empty queue).
 * In:  none
 * Out: none
 * Evidence: rts at $012DC0; beq.w from Layer_FlushTileQueue and beq.s from the run's tail
 */
void Layer_WriteTilesEnd(void)
{
}

/* Trigger_LoadX - loads a stage's horizontal trigger table: insertion-sorts its entries by x key
 * into g_TriggerList (ascending; equal keys keep the table's order), behind a 0 element and ended
 * by the $FFFF sentinel; Trigger_RunX starts at the first real element.
 * In:  table = word count, then the entries (TriggerEntry, each +0 its size) (a0)
 * Out: g_TriggerList, g_TriggerNext = its second element, g_TriggerTable = base = the first entry
 *      (d6); count = $FFFF (the dbra's end, d7's low word); table = past the last entry; end = the
 *      sentinel (a1); src/dst = the last insertion's (a2/a3: unchanged for an empty table)
 * Evidence: clr.l (a1)+ / move.l a1,(g_TriggerNext).w / move.w (a0)+,d7 / the move.l -(a2),-(a3) /
 *           cmp.w (a2),d0 / bcs loop / move.l #$FFFF0000,(a1); 12 stage setups ($01FBA0, $033302,
 *           $040216, ...), 19 corpus runs
 */
void Trigger_LoadX(uint32_t *base, uint16_t *count, const u8 **table, TriggerKey **end, TriggerKey **src,
                   TriggerKey **dst)
{
    TriggerKey *list = (TriggerKey *)g_TriggerList;
    list->key = 0;                           /* the 0 element: every key sorts after it */
    list->offset = 0;
    *end = list + 1;
    g_TriggerNext = ADDR68(*end);
    *count = *(const u16 *)*table;
    *table += sizeof(u16);
    g_TriggerTable = ADDR68(*table);
    *base = ADDR68(*table);
    for (uint16_t n = *count; n-- != 0;) {
        const TriggerEntry *e = (const TriggerEntry *)*table;
        *src = *end;
        *dst = *end + 1;
        *end = *dst;
        uint16_t key = e->x_key;
        do {
            *--*dst = *--*src;               /* shift up the element below */
        } while (key < (*src)->key);         /* cmp.w (a2),d0 / bcs: unsigned */
        (*dst)->key = key;                   /* over the copy of the element it stops at */
        (*dst)->offset = (uint16_t)(ADDR68(e) - *base);
        ++*dst;
        *table += (int16_t)(uint16_t)e->size;
    }
    *count = 0xFFFF;
    (*end)->key = 0xFFFF;
    (*end)->offset = 0;
}
