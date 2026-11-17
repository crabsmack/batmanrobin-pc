#include "game.h"

/* The timed and B-style health drops ($01306A-$0130AF): spawn actions 41-47, each an a4 effect
 * script run by the shared tail Drop_RunFxScript. B pickups are the child $13220 (their own launch
 * and animation; sound $12 and Drop_CollectHealth like the A ones). */

/* their a4 scripts (ROM data after the code; batmanrobin.asm) */
static constexpr uint32_t Scr4_013144 = 0x013144u;   /* five A pickups, $40 apart */
static constexpr uint32_t Scr4_01313A = 0x01313Au;   /* one B */
static constexpr uint32_t Scr4_013132 = 0x013132u;   /* two */
static constexpr uint32_t Scr4_01312A = 0x01312Au;   /* three */
static constexpr uint32_t Scr4_01311A = 0x01311Au;   /* five */
static constexpr uint32_t Scr4_0130FA = 0x0130FAu;   /* nine */
static constexpr uint32_t Scr4_0131B2 = 0x0131B2u;   /* a $380 wait, then five B, one by one */

/* lea script,a4 / bra.w Drop_RunFxScript */
static void run_drop(uint32_t script, uint32_t *a4)
{
    *a4 = script;
    dk::jump(at_Drop_RunFxScript);
}

/* Drop_HealthARow - spawn action 41: releases five A-style health pickups one after another ($40
 * apart, each with its own spawn argument).
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_013144 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_013144,a4 / bra.w Drop_RunFxScript; $576A waits and five $5606 forks into the
 *           A pickup past its launch ($13268, args $34/$28/$1C/$10/$04); 21 TAS runs
 */
void Drop_HealthARow(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                     uint32_t *a6)
{
    run_drop(Scr4_013144, script);
}

/* Drop_HealthB1 - spawn action 42: drops one B-style health pickup (also cases 0 and 4 of
 * Drop_RandomHealthB).
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_01313A (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_01313A,a4 / bra.w Drop_RunFxScript; Tbl_012EF0 entry 42
 */
void Drop_HealthB1(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_01313A, script);
}

/* Drop_HealthB2 - spawn action 43: drops two B-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_013132 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_013132,a4 / bra.w Drop_RunFxScript; 11 corpus runs
 */
void Drop_HealthB2(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_013132, script);
}

/* Drop_HealthB3 - spawn action 44: drops three B-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_01312A (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_01312A,a4 / bra.w Drop_RunFxScript; 12 corpus runs
 */
void Drop_HealthB3(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_01312A, script);
}

/* Drop_HealthB5 - spawn action 45: drops five B-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_01311A (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_01311A,a4 / bra.w Drop_RunFxScript; 13 corpus runs
 */
void Drop_HealthB5(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_01311A, script);
}

/* Drop_HealthB9 - spawn action 46: drops nine B-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0130FA (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0130FA,a4 / bra.w Drop_RunFxScript; the TAS and one fuzz run
 */
void Drop_HealthB9(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_0130FA, script);
}

/* Drop_HealthBRow - spawn action 47: after a $380 wait releases five B-style health pickups one
 * after another.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0131B2 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0131B2,a4 / bra.w Drop_RunFxScript; $576A waits and five forks into the B
 *           pickup past its launch ($13232); run once, in the TAS
 */
void Drop_HealthBRow(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                     uint32_t *a6)
{
    run_drop(Scr4_0131B2, script);
}
