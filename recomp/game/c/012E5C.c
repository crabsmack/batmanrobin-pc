#include "game.h"

/* The vertical stage's triggers ($012E5C-$012ECD): Trigger_LoadX / Trigger_RunX by y, the stage
 * scrolling up, so the list runs from the largest key down. */

/* Trigger_LoadY - loads a vertical-stage trigger table: insertion-sorts its entries by y key into
 * g_TriggerList, largest first (equal keys keep the table's order), behind a $FFFF head element
 * and ended by a 0 element.
 * In:  table = word count, then the entries (TriggerEntry) (a0)
 * Out: g_TriggerList, g_TriggerNext = its second element, g_TriggerTable = base = the first entry
 *      (d6); dst = past the last element inserted (a3; unchanged for an empty table)
 * Evidence: move.l #$FFFF0000,(a1)+ / move.w $4(a0),d0 / cmp.w (a2),d0 / bhi loop / clr.l (a1);
 *           only caller the zeppelin stage setup $032F14 (Tbl_02105A); TAS frame 21157
 */
void Trigger_LoadY(uint32_t *base, const u8 *table, TriggerKey **dst)
{
    TriggerKey *list = (TriggerKey *)g_TriggerList;
    list->key = 0xFFFF;                      /* the head: every key sorts after it */
    list->offset = 0;
    TriggerKey *end = list + 1;
    g_TriggerNext = ADDR68(end);
    uint16_t count = *(const u16 *)table;
    table += sizeof(u16);
    g_TriggerTable = ADDR68(table);
    *base = ADDR68(table);
    while (count-- != 0) {                   /* dbra d7 */
        const TriggerEntry *e = (const TriggerEntry *)table;
        TriggerKey *src = end;
        *dst = end + 1;
        end = *dst;
        uint16_t key = e->y_key;
        do {
            *--*dst = *--src;                /* shift up the element below */
        } while (key > src->key);            /* cmp.w (a2),d0 / bhi: unsigned */
        (*dst)->key = key;
        (*dst)->offset = (uint16_t)(ADDR68(e) - *base);
        ++*dst;
        table += (int16_t)(uint16_t)e->size;
    }
    end->key = 0;                            /* clr.l: the end element */
    end->offset = 0;
}

/* Trigger_RunY - per frame on the vertical stage: fires the triggers whose y key lies in the
 * screen (top < key < top + $E0, top = g_MapScrollY's integer part), in list order (largest
 * first); keys below the screen are skipped for good, and the walk stops at the first key at or
 * above the screen's top, or at a callback that asks to be retried. Nothing while the
 * screen-clear pause runs.
 * In:  g_TriggerNext, g_TriggerTable, g_MapScrollY; g_ScreenClearTimer
 * Out: g_TriggerNext = the first element not passed; top/bottom (d6/d7 low words); next/table
 *      (a2/a3); d0 = the last element read, swapped (Trigger_Fire's); the callbacks' registers
 * Evidence: move.w (g_MapScrollY).w,d6 / addi.w #$E0,d7 / cmp.w d6,d0 / bls / cmp.w d7,d0 / bcc /
 *           bsr Trigger_Fire / beq; pointed to from the vertical stage code $032FC4; 20 corpus runs
 */
void Trigger_RunY(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *top,
                  uint32_t *bottom, uint32_t *a1, const TriggerKey **next, const u8 **table, uint32_t *a4,
                  uint32_t a5, uint32_t a6)
{
    if (g_ScreenClearTimer != 0)
        return;
    PTR(const TriggerKey) n;
    n.v = (uint32_t)g_TriggerNext;
    *next = n;
    PTR(const u8) t;
    t.v = (uint32_t)g_TriggerTable;
    *table = t;
    uint16_t y = (uint16_t)(g_MapScrollY >> 16);
    *top = (*top & 0xFFFF0000u) | y;
    *bottom = (*bottom & 0xFFFF0000u) | (uint16_t)(y + 0xE0);
    uint32_t callback = 0, sprite = a5, spawned = a6;   /* a0, a5, a6: no caller reads them after */
    for (;;) {
        const TriggerKey *k = (*next)++;
        uint16_t key = k->key;
        *d0 = ((uint32_t)(uint16_t)k->offset << 16) | key;   /* move.l (a2)+,d0 / swap */
        if (key <= (uint16_t)*top)
            break;                                            /* not on screen yet */
        if (key >= (uint16_t)*bottom)
            continue;                                         /* already below the screen */
        dk::Flags fired = {};
        Trigger_Fire(d0, d1, d2, d3, d4, d5, *top, *bottom, &callback, a1, ADDR68(*next), *table, a4, &sprite,
                     &spawned, &fired);
        if (!fired.z)
            break;                                            /* retried next frame */
    }
    --*next;
    g_TriggerNext = ADDR68(*next);
}
