#include "game.h"

enum { FLIGHT_SCROLL_LINES = 0x0C };   /* g_ScrollMode: per-line H-scroll of both planes */

/* Flight_StartSkyOnly - loader-script native of the $2400/$2401 restarts: scroll mode $C and the
 * frame callback Flight_DrawSky (the sky backdrop only, no corridor or cloud bank)
 * In:  -
 * Out: d0 = 0 and Z set (Op6_CallNativeRetry: done)
 * Evidence: move.w #$C,d0 / Scroll_SetMode / move.l #Flight_DrawSky,g_SceneVars+$98 / moveq #0,d0;
 *           `70BE 0003 3382` at $331B4 (Scr6_033182) and $3323C (Scr6_0331F0); level-skip scenario
 *           f5894-f6633
 */
uint32_t Flight_StartSkyOnly(void)
{
    Scroll_SetMode(FLIGHT_SCROLL_LINES);
    g_FlightScene->frame_fn = at_Flight_DrawSky;
    return 0;
}
