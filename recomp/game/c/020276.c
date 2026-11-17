#include "game.h"

/* the two launchers' balloon height tables (ROM data, batmanrobin.asm) */
static constexpr uint32_t Tbl_020354 = 0x020354u;
static constexpr uint32_t Tbl_02036E = 0x02036Eu;

/* Stage1_BalloonUseHeightsA - tail of Stage1_SpawnBalloonLauncherB: points the new launcher at the
 * balloon height table Tbl_020354 and reports success
 * In:  self = the launcher just spawned (a6)
 * Out: self->heights = Tbl_020354; d0 = 0, Z set
 * Evidence: move.l #$20354,$1E(a6) / moveq #0,d0 / rts; jmp from $020090; $1E(a6) is the cursor
 *           Stage1_LaunchBalloonA/B read
 */
uint32_t Stage1_BalloonUseHeightsA(BalloonLauncherObj *self)
{
    self->heights.v = Tbl_020354;
    return 0;
}

/* Stage1_BalloonUseHeightsB - tail of Stage1_SpawnBalloonLauncherA: points the new launcher at the
 * balloon height table Tbl_02036E and reports success
 * In:  self = the launcher just spawned (a6)
 * Out: self->heights = Tbl_02036E; d0 = 0, Z set
 * Evidence: move.l #$2036E,$1E(a6) / moveq #0,d0 / rts; jmp from $01FFB2; RAM f13800: launcher $1E =
 *           $0002037A (3 pairs into Tbl_02036E). A never-run third variant at $02028E (raw) sets $020388
 */
uint32_t Stage1_BalloonUseHeightsB(BalloonLauncherObj *self)
{
    self->heights.v = Tbl_02036E;
    return 0;
}
