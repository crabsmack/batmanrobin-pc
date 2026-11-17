#include "game.h"

/* the heading thresholds, steepest first: one case per band (5 cases, the last = <= $1F4) */
static constexpr uint16_t BOLT_BANDS[4] = { 0x20C, 0x204, 0x1FC, 0x1F4 };

/* Boss_BeamShip_BoltFrameByHeading - a4 switch native: picks the ship's laser bolt sprite by its
 * heading near due left ($200): > $20C, > $204, > $1FC, > $1F4, else (5 cases: the bolt tilted by
 * up to 2 steps either way)
 * In:  bolt = the bolt (a6: Vm4Obj heading); script = 5 word case offsets, each from itself (a4)
 * Out: d0.w = the heading & $3FE; script = the chosen case; ccr = the last cmpi's (adda keeps it)
 * Evidence: move.w $2A(a6),d0 / andi.w #$3FE,d0 / cmpi.w #$20C,d0 / bgt / addq.l #2,a4 ... cmpi.w
 *           #$1F4,d0 / bgt / addq.l #2,a4 / adda.w (a4),a4; op $5438 (Op4_CallNative) at $029840 in
 *           the bolt script $02980C; cases draw frames $3D4A/$3D40/$3D36; TAS f77950-f78522. The
 *           prototype gains &ccr for the measured ccout(NZVC)
 */
void Boss_BeamShip_BoltFrameByHeading(uint16_t *d0, const s16 **script, const Vm4Obj *bolt, dk::Flags *ccr)
{
    uint16_t heading = (uint16_t)((uint16_t)bolt->heading & 0x3FE);
    *d0 = heading;
    for (uint16_t band : BOLT_BANDS) {
        *ccr = dk::Flags::compare(heading, band, 2, ccr->x);
        if ((int16_t)heading > (int16_t)band)   /* bgt: signed */
            break;
        (*script)++;                            /* addq.l #2,a4: the next case */
    }
    *script = (const s16 *)((const u8 *)*script + (int16_t)**script);   /* adda.w (a4),a4 */
}
