#include "game.h"

enum {
    SCROLL_ROAD = 0x28,        /* g_ScrollMode: plane A line DMA + the H-int V-scroll raster */
};

/* Stage3_InitHatterRoad - sets up the Hatter road scene: scroll mode $28, the plane-A V-scroll
 * H-int, then the road driver, the swirl at the road's far end and the Hatter boss
 * In:  d4/d5 = position (passed to the spawners); a3/a4 as the spawners leave them
 * Out: g_ScrollMode; the three objects; a3/a4 = the boss's blocks
 * Evidence: move.w #$28,(g_ScrollMode).l / jsr HInt_InstallVScrollA / bsr.w Stage3_SpawnRoadDriver /
 *           jsr Stage3_SpawnRoadEndSwirl / jsr Boss_Hatter_Spawn / rts; bsr from
 *           Stage3_RunHatterRoad only; TAS f113311
 */
void Stage3_InitHatterRoad(uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t a6)
{
    g_ScrollMode = SCROLL_ROAD;
    uint32_t d7 = 0, a0 = 0, a1 = 0, a2 = 0;
    HInt_InstallVScrollA(&d7, &a0, &a1);
    Stage3_SpawnRoadDriver(d4, d5, &a2, a3, a4, &a6);
    Stage3_SpawnRoadEndSwirl(d4, d5, a3, a4, &a6);
    Boss_Hatter_Spawn(d4, d5, a3, a4, a6, &a2);
}
