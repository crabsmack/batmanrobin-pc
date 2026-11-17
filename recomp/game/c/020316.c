#include "game.h"

/* the balloon bomb's a2 script and the height table this launcher wraps to (ROM) */
static constexpr uint32_t Scr2_0203E0 = 0x0203E0u;
static constexpr uint32_t Tbl_020388 = 0x020388u;

/* the launch of Stage1_LaunchBalloonA/B (02029A.c), the same code: spawn a balloon bomb at the launcher and hand it the next pair of heights,
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

/* Stage1_LaunchBalloonC - launcher native: the balloon-bomb spawn of Stage1_LaunchBalloonA, its
 * height cursor wrapping to the third table Tbl_020388
 * In:  self = the launcher (a6), heights = the height-pair cursor; a4 for Obj_SpawnEnemy
 * Out: as Stage1_LaunchBalloonA
 * Evidence: Stage1_LaunchBalloonA/B's code with lea Tbl_020388,a0; Op2_CallNativeRel at $02024A,
 *           $020254, $02025E (a launcher script's three timed launches); no corpus run spawns that
 *           launcher (crane-crash 2026-10-30); 020316.syn checks it
 */
void Stage1_LaunchBalloonC(uint32_t *d0, uint16_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a1,
                           uint32_t a4, BalloonLauncherObj *self)
{
    launch_balloon(Tbl_020388, d0, d1, d4, d5, d7, a1, a4, self);
}
