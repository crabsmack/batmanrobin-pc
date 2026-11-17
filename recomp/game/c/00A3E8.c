#include "game.h"

/* Pad_Update - reads one pad (Pad_ReadPort) into its record: held now, held before, newly pressed
 * (held and not before), newly released (before and not now); the record pointer moves past it.
 * In:  port (a4) = the pad's data register, rec (a5)
 * Out: *rec updated; rec + 1 (the next pad's record)
 * Evidence: bsr Pad_ReadPort / move.b (a5),d1 / move.b d0,(a5)+ / move.b d1,(a5) / not.b d1 / and.b
 *           d0,d1 / not.b d0 / and.b (a5)+,d0 / move.b d1,(a5)+ / move.b d0,(a5)+; callers
 *           Pad_ReadAll (twice) and Pad_ReadLive (once, then falls into it)
 */
void Pad_Update(uint32_t port, PadRec **rec)
{
    uint8_t now = Pad_ReadPort(port);
    PadRec *r = *rec;
    uint8_t before = r->held;
    r->held = now;
    r->prev = before;
    r->pressed = (uint8_t)(~before & now);
    r->released = (uint8_t)(~now & before);
    *rec = r + 1;
}
