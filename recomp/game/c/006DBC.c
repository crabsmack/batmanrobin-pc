#include "game.h"

/* Op5_SetQuota / Op5_SetPopulationCap / Op5_ClearOrigin ($006DBC-$006DEB): the wave director's
 * spawn quota, population caps and spawn origin. */

static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* Op5_SetQuota - a5 op: sets the wave's spawn quota (one player / two players) and the script
 * position it goes to once that many enemies spawned; clears the spawn count.
 * In:  script = word quota 1P, word quota 2P, word offset (from its own word); a6 = the wave
 * Out: self->quota_1p / quota_2p / after_quota; self->spawned = 0; a0 = the handler; NEXT
 * Evidence: move.w (a5)+,$E(a6) / move.w (a5)+,$10(a6) / movea.l a5,a0 / adda.w (a5)+,a0 /
 *           move.l a0,$16(a6) / clr.w $C(a6); the spawn ops' tail $7052 compares $C with the quota
 *           and resumes at $16; script uses $21DAE (`6DBC 01F4 01F4 004A`), $21DEC; 4 corpus runs
 */
void Op5_SetQuota(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    self->quota_1p = *(*script)++;
    self->quota_2p = *(*script)++;
    self->after_quota = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    (*script)++;
    self->spawned = 0;
    next(handler, script);
}

/* Op5_SetPopulationCap - a5 op: sets the population caps (one player, two players): the wave only
 * counts down and spawns while g_ObjPopulation is below the cap.
 * In:  script = word cap 1P, word cap 2P; a6 = the wave
 * Out: self->cap_1p / cap_2p; NEXT
 * Evidence: move.w (a5)+,$12(a6) / move.w (a5)+,$14(a6) / NEXT; read by VM5_RunObject; 74 script
 *           uses, e.g. $1693C (`6DD4 0003 0004`); 29 corpus runs
 */
void Op5_SetPopulationCap(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    self->cap_1p = *(*script)++;
    self->cap_2p = *(*script)++;
    next(handler, script);
}

/* Op5_ClearOrigin - a5 op: clears the wave's spawn origin offset (added to every spawn entry's x /
 * y).
 * In:  script (no operands); a6 = the wave
 * Out: self->origin_x = origin_y = 0; NEXT
 * Evidence: clr.w $26(a6) / clr.w $28(a6) / NEXT; read by VM5_SpawnEntry ($6FEA); script uses
 *           $21DC4, $21DD4; 37 corpus runs
 */
void Op5_ClearOrigin(uint32_t *handler, const u16 **script, Vm5Wave *self)
{
    self->origin_x = 0;
    self->origin_y = 0;
    next(handler, script);
}
