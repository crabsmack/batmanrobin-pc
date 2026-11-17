#include "game.h"

/* the empty x-trigger table: a count word of 0 (ROM) */
static constexpr uint32_t Tbl_012ECE = 0x012ECEu;

enum { FLIGHT_SCROLL_CORRIDOR = 0x34 };   /* g_ScrollMode: plane B lines + the H-int V-scroll table */

static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Flight_StartCorridorNoWaves - Flight_StartCorridor for the $2300 restart: the same corridor
 * scroll mode $34, H-int and frame callback, but the empty trigger table Tbl_012ECE, so no
 * corridor wave starts
 * In:  -
 * Out: d0 = 0 and Z set (Op6_CallNativeRetry: done); Trigger_LoadX's d6/d7/a0-a3 (an empty table
 *      leaves its src/dst as they came), then HInt_InstallVScrollB's d7/a0/a1
 * Evidence: lea Tbl_012ECE,a0 / Trigger_LoadX / move.w #$34,d0 / Scroll_SetMode /
 *           HInt_InstallVScrollB / move.l #Flight_FrameCorridor,g_SceneVars+$98 / moveq #0,d0;
 *           `70BE 0003 3322` at $3314C in Scr6_033120 (Stage2_RunFlightClouds); level-skip f5161
 */
void Flight_StartCorridorNoWaves(uint32_t *d0, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3)
{
    /* Trigger_LoadX on the empty table: a count of 0 inserts nothing, so its src/dst (a2/a3)
       stay as the registers had them; only end (a1) and the table pointer (a0) are written */
    PTR(const u8) rom;
    rom.v = Tbl_012ECE;
    const u8 *table = rom.get();
    TriggerKey *list = (TriggerKey *)g_TriggerList;
    TriggerKey *e = list, *s = list, *d = list;   /* s/d: not written for an empty table */
    uint16_t count = (uint16_t)*d7;
    Trigger_LoadX(d6, &count, &table, &e, &s, &d);
    set_low_word(d7, count);
    *a0 = dk::addr(table);
    *a1 = dk::addr(e);
    set_low_word(d0, FLIGHT_SCROLL_CORRIDOR);
    Scroll_SetMode((uint16_t)*d0);
    HInt_InstallVScrollB(d7, a0, a1);
    g_FlightScene->frame_fn = at_Flight_FrameCorridor;
    *d0 = 0;
}
