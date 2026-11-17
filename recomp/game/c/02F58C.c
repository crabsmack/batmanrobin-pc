#include "game.h"

/* Flight_AirshipRunTriggers - the airship hull's trigger runner, in map space: fires the
 * g_TriggerList entries whose x key lies in [g_CameraX + map_x, + $17C] in key order, and places
 * every object a trigger spawned at the entry's map position (drawn position = the entry's x/y,
 * world position from Flight_AirshipMapToWorld). Unlike Trigger_RunX the bounds are signed, a key
 * left of the window is skipped for good, and an element with a negative offset ends the list.
 * In:  g_TriggerNext / g_TriggerTable (Tbl_0307C2 via Flight_AirshipStartPath), g_CameraX,
 *      g_FlightAirship map_x/map_y; the registers the trigger callbacks read
 * Out: g_TriggerNext = the first element not passed; left/right = the window (d6/d7 low words);
 *      d0 = the last element read, swapped (Trigger_Fire's after a firing); d4/d5 = the last placed
 *      object's world position; d1/d2/d5 as the callbacks leave them; a1-a6 restored (movem)
 * Evidence: movem.l a1-a6 / move.l (a2)+,d0 / swap / bmi / cmp.w d7,d0 / bgt / cmp.w d6,d0 / blt /
 *           jsr Trigger_Fire / bne / $3E,$42 -> $4E,$50 / bsr Flight_AirshipMapToWorld / subq.l
 *           #4,a2; fall-through from Flight_AirshipSetMapOrigin; TAS 67264-77147
 */
void Flight_AirshipRunTriggers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5,
                               uint32_t *left, uint32_t *right, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                               uint32_t a5, uint32_t a6)
{
    /* movem.l a1-a6,-(a7): on the 68000's stack, so the callbacks' frames sit as deep as the
       original's; the restore at the end is the bridge's (a1-a6 are not this prototype's outputs) */
    dk::push_long(a6);
    dk::push_long(a5);
    dk::push_long(a4);
    dk::push_long(a3);
    dk::push_long(a2);
    dk::push_long(a1);

    PTR(const TriggerKey) n;
    n.v = (uint32_t)g_TriggerNext;
    const TriggerKey *next = n;
    PTR(const u8) t;
    t.v = (uint32_t)g_TriggerTable;
    const u8 *table = t;
    uint16_t lo = (uint16_t)((uint16_t)(g_CameraX >> 16) + (uint16_t)g_FlightAirship->map_x);
    uint16_t hi = (uint16_t)(lo + 0x17C + WS_WIN_R());   /* native: + the view right of 4:3 (R4, R20), as Trigger_RunX */
    *left = (*left & 0xFFFF0000u) | lo;
    *right = (*right & 0xFFFF0000u) | hi;
    dk::Flags flags = {};
    flags.x = (uint32_t)lo + 0x17Cu > 0xFFFFu;   /* addi.w: X is the carry; nothing after it sets X */

    /* the callbacks' scratch registers: a1-a6 come back as they were (the movem restore) */
    uint32_t r_a0 = 0, r_a1 = a1, r_a4 = a4, r_a5 = a5, r_a6 = a6;
    for (;;) {
        const TriggerKey *k = next++;
        uint16_t key = k->key;
        *d0 = ((uint32_t)(uint16_t)k->offset << 16) | key;   /* move.l (a2)+,d0 / swap */
        if ((int32_t)*d0 < 0)
            break;                                            /* bmi: the end marker's offset */
        if ((int16_t)key > (int16_t)hi)
            break;                                            /* not in the window yet */
        if ((int16_t)key < (int16_t)lo)
            continue;                                         /* behind the window */
        Trigger_Fire(d0, d1, d2, d3, d4, d5, *left, *right, &r_a0, &r_a1, dk::addr(next), table, &r_a4, &r_a5,
                     &r_a6, &flags);
        if (!flags.z)
            break;                                            /* the callback asked to be retried */
        PTR(Vm2Obj) obj;
        obj.v = r_a6;                                         /* the object the trigger spawned */
        obj->draw_x = obj->x;                                 /* its map position */
        obj->draw_y = obj->y;
        uint16_t wx = (uint16_t)*d4, wy = (uint16_t)*d5;
        Flight_AirshipMapToWorld(&wx, &wy, r_a6);
        *d4 = (*d4 & 0xFFFF0000u) | wx;
        *d5 = (*d5 & 0xFFFF0000u) | wy;
    }
    --next;
    g_TriggerNext = dk::addr(next);

    for (int i = 0; i < 6; i++)
        dk::pop_long();                                       /* movem.l (a7)+,a1-a6 */
}
