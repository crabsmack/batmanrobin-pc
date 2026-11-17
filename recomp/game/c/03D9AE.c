#include "game.h"

/* the little hat's script after the native's jsr $7FE.w (VM2_Enter): Op2_Kill */
static constexpr uint32_t Scr2_03D9BC = 0x03D9BCu;

enum {
    SPAWN_ACTION_EXTRA_LIFE = 0x1C,   /* Tbl_012EF0 entry 28: Item_SpawnExtraLifeFromHit */
};

/* Boss_Hatter_LittleHatDropLife - a2 native fragment when the little hat is shot down: runs spawn
 * action $1C (an extra life) at its position, then the script goes on and kills it
 * In:  a6 = the little hat; the rest pass through to SpawnAction_Run
 * Out: d0-d7/a1 as SpawnAction_Run left them (d4/d5 = the hat's x/y, ...), into the script at
 *      Scr2_03D9BC: VM2_Enter runs on with them, so they are outputs (verify, TAS call 1: d4
 *      generated $01A601A6, C $01A60000 when they were inputs only)
 * Evidence: move.w #$1C,d0 / jsr SpawnAction_Run / jsr ($7FE).w; after Op2_TakeHit and
 *           Op2_FxSpawnOffAbs in Scr2_03D940; TAS f115744 (png f115800: 1UP beside Batman)
 */
void Boss_Hatter_LittleHatDropLife(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                                   uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                   uint32_t a5, uint32_t a6)
{
    *d0 = (*d0 & 0xFFFF0000u) | SPAWN_ACTION_EXTRA_LIFE;
    SpawnAction_Run(d0, d1, d2, d3, d4, d5, d6, d7, a1, a2, a3, a4, a5, a6);
    dk::jump_script(at_VM2_Enter, Scr2_03D9BC);
}
