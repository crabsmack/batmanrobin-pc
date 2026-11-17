#include "game.h"

static constexpr uint16_t CLAW_HIT_PAUSE = 8;   /* frames the claw holds still after landing a hit */

/* Boss_Claw_HitCallback - hit callback of the claw's attack box: EnemyAtk_HitKnock, and when it hit,
 * holds the claw for 8 frames (hit_pause, counted down by Boss_Claw_TickHitPause)
 * In:  as EnemyAtk_HitKnock (box = the struck box a2, a6 = the claw)
 * Out: EnemyAtk_HitKnock's registers; the claw's hit_pause = 8 when it hit (Z set)
 * Evidence: jsr EnemyAtk_HitKnock / bne / move.w #8,$30(a6) / rts; callback long $0001E768 with
 *           args $30/$380 in the Op2_SetAttackBox spec at $1E77C (Scr2_01E778); TAS f12387-f12548
 *           (11 frames)
 */
void Boss_Claw_HitCallback(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                           uint32_t *d6, uint32_t *d7, uint32_t *a0, HitBox *box, uint32_t a3, uint32_t *a4,
                           uint32_t a5, uint32_t *a6)
{
    dk::Flags hit{};
    EnemyAtk_HitKnock(d0, d1, d2, d3, d4, d5, d6, d7, a0, box, a3, a4, a5, a6, &hit);
    if (hit.z) {
        PTR(ClawObj) claw;
        claw.v = *a6;
        claw->hit_pause = CLAW_HIT_PAUSE;
    }
}
