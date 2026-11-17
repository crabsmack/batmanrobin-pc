#include "game.h"

/* the balloon bomb's a2 script and the two height tables the launchers wrap to (ROM) */
static constexpr uint32_t Scr2_0203E0 = 0x0203E0u;
static constexpr uint32_t Tbl_020354 = 0x020354u;
static constexpr uint32_t Tbl_02036E = 0x02036Eu;

/* both launch natives: spawn a balloon bomb at the launcher and hand it the next pair of heights,
 * the cursor wrapping to `wrap` when the pair after it starts with a negative word. The movem.l
 * a2-a6 around Obj_SpawnEnemy keeps the launcher's registers; only a1 carries the balloon out. */
static void launch_balloon(uint32_t wrap, uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7,
                           uint32_t *a1, uint32_t a4, BalloonLauncherObj *self)
{
    *d4 = (uint16_t)self->x;
    *d5 = (uint16_t)self->y;
    uint32_t a0 = 0, a3 = 0, a5 = 0;  /* restored by the movem (a0 clobbered) */
    uint32_t r4 = a4, r6 = ADDR68(self);
    Obj_SpawnEnemy(d0, d1, *d4, *d5, d7, &a0, Scr2_0203E0, &a3, &r4, &a5, &r6);
    *a1 = r6;                         /* movea.l a6,a1: the balloon (the launcher when the pool was short) */
    if (*d0 != 0)                     /* bne: the pool was short (d0 = -1, Z clear) */
        return;
    PTR(BalloonBombObj) spawned;
    spawned.v = r6;
    BalloonBombObj *balloon = spawned;
    const BalloonHeights *pair = self->heights;
    balloon->height[0] = pair->height[0];
    balloon->height[1] = pair->height[1];
    pair++;
    if (pair->height[0] < 0)          /* tst.w (a0) / bpl: the end marker */
        self->heights.v = wrap;
    else
        self->heights = pair;
    *d0 = 0;
}

/* Stage1_LaunchBalloonA - launcher native: spawns a balloon bomb (Scr2_0203E0) at the launcher's
 * position and gives it the next pair of bob heights from the launcher's table, wrapping to
 * Tbl_020354 at the end marker
 * In:  self = the launcher (a6): heights = the cursor into (height1, height2) word pairs ended by a
 *      negative word; a4 for Obj_SpawnEnemy
 * Out: d4/d5 = the launcher's x/y; a1 = the balloon, its height[0..1] = the pair; the cursor
 *      advanced; d0 = 0. When the pool is short: d0 = Obj_SpawnEnemy's nonzero result, nothing set
 * Evidence: move.w $3E/$42(a6),d4/d5 / movem.l a2-a6,-(a7) / lea Scr2_0203E0,a2 / jsr
 *           Obj_SpawnEnemy / movea.l a6,a1 / movem.l (a7)+,a2-a6 / bne / 2 x move.w (a0)+ to
 *           $2A/$2C(a1) / tst.w (a0) / bpl / lea Tbl_020354,a0 / move.l a0,$1E(a6) / moveq #0,d0;
 *           op $2CB2 from Scr2_020094 and Scr2_01FFB8; TAS f13918-f18166 (84 frames); RAM f14400:
 *           balloons with $2A/$2C = $32/$AF, $4B/$AF, $64/$C8
 */
void Stage1_LaunchBalloonA(uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a1,
                           uint32_t a4, BalloonLauncherObj *self)
{
    launch_balloon(Tbl_020354, d0, d1, d4, d5, d7, a1, a4, self);
}

/* Stage1_LaunchBalloonB - launcher native: the same balloon-bomb spawn as Stage1_LaunchBalloonA, but
 * its height cursor wraps to Tbl_02036E
 * In:  self = the launcher (a6), heights = the height-pair cursor; a4 for Obj_SpawnEnemy
 * Out: as Stage1_LaunchBalloonA
 * Evidence: the same code with lea Tbl_02036E,a0; op $2CB2 from Scr2_01FFB8 ($01FFE2...) and
 *           Scr2_020094 ($0200F6...); the TAS and fuzz-14400..18000. A never-run copy at $020316
 *           (raw) wraps to $020388
 */
void Stage1_LaunchBalloonB(uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a1,
                           uint32_t a4, BalloonLauncherObj *self)
{
    launch_balloon(Tbl_02036E, d0, d1, d4, d5, d7, a1, a4, self);
}
