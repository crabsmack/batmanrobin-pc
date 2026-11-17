#include "game.h"

/* Flight_CopyBandLines - corridor helper: copies lines of the middle wall, the H-scroll from the
 * band table (the high word of each long) and a constant V-scroll, cut at the line budget; when
 * the budget runs out it also ends Flight_DrawCorridor (it drops that return address)
 * In:  vscroll = d2.w; *count = d7.w (lines copied: dbra's count, d7 itself, not d7 + 1);
 *      hlines/vlines = the H-scroll and H-int V-scroll line tables (a0/a1); bands = the band
 *      longs (a4); g_FlightCorridor->lines_left
 * Out: lines_left -= count; the budget ran out: count cut to what was left, the return goes to
 *      Flight_DrawCorridor's caller; the tables and bands advanced; d7.w = $FFFF; the flags of the
 *      last move.w d2,(a1)+ (or of the sub/add when nothing was copied)
 * Evidence: sub.w d7,g_SceneVars+$10 / bgt / add.w g_SceneVars+$10,d7 / addq.l #4,a7 / dbra loop:
 *           move.w (a4)+,(a0)+ / move.w d2,(a1)+ / addq.l #2,a4; bsr from Flight_DrawCorridor
 *           ($0249FE); TAS f35433-f47954. The prototype gains &ccr for the measured ccout(NZVC)
 */
void Flight_CopyBandLines(uint16_t vscroll, uint16_t *count, u16 **hlines, u16 **vlines, const u16 **bands,
                          dk::Flags *ccr)
{
    FlightCorridorVars *v = g_FlightCorridor;
    uint16_t left = (uint16_t)v->lines_left;
    uint16_t after = (uint16_t)(left - *count);
    v->lines_left = (int16_t)after;
    *ccr = dk::Flags::compare(left, *count, 2);   /* sub.w: X = C */
    ccr->x = ccr->c;
    bool gt = !ccr->z && (ccr->n == ccr->v);
    if (!gt) {
        uint32_t sum = (uint32_t)*count + after;   /* add.w g_SceneVars+$10,d7 */
        uint16_t r = (uint16_t)sum;
        bool vflag = ((~(*count ^ after) & (*count ^ r)) & 0x8000) != 0;
        *count = r;
        *ccr = dk::Flags{ sum > 0xFFFFu, (r & 0x8000) != 0, r == 0, vflag, sum > 0xFFFFu };
        dk::drop_return();   /* addq.l #4,a7: the rts goes to Flight_DrawCorridor's caller */
    }
    while ((*count)-- != 0) {   /* dbra d7 */
        *(*hlines)++ = **bands;
        *(*vlines)++ = vscroll;
        *bands += 2;            /* the long's low word skipped */
        *ccr = dk::Flags::test(vscroll, 2, ccr->x);
    }
}

/* the thug's a2 script (ROM data right after this spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_024AE2 = 0x024AE2u;

/* Enemy_BarrelThug_SpawnA - lift trigger callback: spawns the barrel-carrying thug Scr2_024AE2
 * (score $64, energy $50/$C0, drop action $25; falls in with op $1CDC, first move after timer
 * $1E0; killed, it drops its barrel). It shares Flight_CopyBandLines' function file
 * In:  x/y = the trigger's position (d4/d5)
 * Out: script = Scr2_024AE2, then Obj_SpawnEnemy: Z set and a6 = the thug when it spawned,
 *      Z clear when the pool is short (d0 is clobbered: no caller reads it)
 * Evidence: lea Scr2_024AE2,a2 / jmp Obj_SpawnEnemy; dc.l entries in Tbl_02105A ($0212A4 ...)
 *           and wave script Tbl_021B26 ($021CF2); the reference ran it in the TAS and 7 fuzz runs
 */
void Enemy_BarrelThug_SpawnA(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a0, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    *script = Scr2_024AE2;
    dk::jump(at_Obj_SpawnEnemy);   /* jmp: Obj_SpawnEnemy returns to our caller */
}
