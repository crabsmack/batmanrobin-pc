#include "game.h"

/* The A-style health drops ($013038-$01305F): spawn actions 36-39, each an a4 effect script that
 * forks that many A pickups (child $13252: sound $12, then Drop_CollectHealth), run by the shared
 * tail Drop_RunFxScript. */

/* their a4 scripts (ROM data after the code; batmanrobin.asm) */
static constexpr uint32_t Scr4_0130F0 = 0x0130F0u;   /* one fork */
static constexpr uint32_t Scr4_0130E8 = 0x0130E8u;   /* two */
static constexpr uint32_t Scr4_0130E0 = 0x0130E0u;   /* three */
static constexpr uint32_t Scr4_0130D0 = 0x0130D0u;   /* five */

/* lea script,a4 / bra.w Drop_RunFxScript */
static void run_drop(uint32_t script, uint32_t *a4)
{
    *a4 = script;
    dk::jump(at_Drop_RunFxScript);
}

/* Drop_HealthA1 - spawn action 36: drops one A-style health pickup.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0130F0 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0130F0,a4 / bra.w Drop_RunFxScript; Tbl_012EF0 entry 36; 39 TAS runs
 */
void Drop_HealthA1(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_0130F0, script);
}

/* Drop_HealthA2 - spawn action 37: drops two A-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0130E8 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0130E8,a4 / bra.w Drop_RunFxScript; two $5606 forks of the $13252 pickup
 */
void Drop_HealthA2(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_0130E8, script);
}

/* Drop_HealthA3 - spawn action 38: drops three A-style health pickups.
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0130E0 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0130E0,a4 / bra.w Drop_RunFxScript; Tbl_012EF0 entry 38
 */
void Drop_HealthA3(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_0130E0, script);
}

/* Drop_HealthA5 - spawn action 39: drops five A-style health pickups (entry 40, nine of them at
 * $013060, is never run and stays raw).
 * In:  x/y = the position (d4/d5)
 * Out: script = Scr4_0130D0 (a4), then Drop_RunFxScript
 * Evidence: lea Scr4_0130D0,a4 / bra.w Drop_RunFxScript; Tbl_012EF0 entry 39
 */
void Drop_HealthA5(uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *a0, uint32_t *script,
                   uint32_t *a6)
{
    run_drop(Scr4_0130D0, script);
}
