#include "game.h"

/* HUD_ShowGoSignCb - native callback: starts the GO! sign (HUD_StartGoSign) and returns 0.
 * In:  none
 * Out: the sign started (x $168, speed -4.0); d0 = 0 (moveq: tst(d0.l)'s flags)
 * Evidence: jsr HUD_StartGoSign / moveq #0,d0 / rts; callers the a5 op $6E9A and script $1EFAC;
 *           TAS f1460 shows GO!. Prototype: tst(d0.l) (the moveq's flags, its ccout)
 */
uint32_t HUD_ShowGoSignCb(void)
{
    HUD_StartGoSign();
    return 0;
}
