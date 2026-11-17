#include "game.h"

/* $0095A2-$0095EB: the screen-shake starters. Each sets the bounce's launch speed and pull
 * (g_ShakeVel / g_ShakeGravity, 16.16) and ends in Shake_Arm, which restarts the bounce from rest
 * and kicks the second oscillator when it has settled near 0. The ...IfIdle entries return at once
 * while a shake runs (g_ShakeY != 0). */

enum {
    SHAKE_ARM_PULL = 0x2000,        /* g_Shake2Pull: the second oscillator's pull, 1/8 px per frame^2 */
    SHAKE_ARM_KICK = 4,             /* g_Shake2Vel's integer part when the kick happens */
    SHAKE_ARM_SETTLED = 0xC,        /* ... and g_Shake2Pos's integer part must be at most this */
};

/* Shake_Arm - the shared tail of the Shake_Start* entries: the bounce restarts from rest, and the
 * second oscillator gets pull $2000 and, unless it is out beyond 12 px, a kick of 4.0.
 * In:  g_ShakeVel / g_ShakeGravity set by the caller
 * Out: g_ShakeY = 0, g_Shake2Pull = $2000; g_Shake2Vel high word = 4 when g_Shake2Pos's is <= 12;
 *      ccr = the cmpi.w's flags (no kick) or the move.w #4's
 * Evidence: clr.l g_ShakeY / move.l #$2000,g_Shake2Pull / cmpi.w #$C,g_Shake2Pos / bgt.s Shake_Done /
 *           move.w #4,g_Shake2Vel; branched to by Shake_Start, Shake_StartLong, Shake_StartBig,
 *           Shake_StartRandom. Prototype: &ccr added (its ccout)
 */
void Shake_Arm(dk::Flags *ccr)
{
    g_ShakeY = 0;
    g_Shake2Pull = SHAKE_ARM_PULL;
    int16_t pos = (int16_t)(g_Shake2Pos >> 16);   /* the word at g_Shake2Pos: the integer part */
    *ccr = dk::Flags::compare((uint16_t)pos, SHAKE_ARM_SETTLED, 2, ccr->x);
    if (pos > SHAKE_ARM_SETTLED)
        return;
    g_Shake2Vel = (uint32_t)SHAKE_ARM_KICK << 16 | (uint16_t)g_Shake2Vel;   /* move.w: the high word */
    *ccr = dk::Flags::test(SHAKE_ARM_KICK, 2, ccr->x);
}

/* Shake_Done - the rts the shake starters share (a shake already running, or no kick).
 * In:  none
 * Out: none (the condition codes stay as the test before left them)
 * Evidence: bne.s here from Shake_StartIfIdle, Shake_StartLongIfIdle, Shake_StartBigIfIdle,
 *           Shake_StartRandomIfIdle; bgt.s in Shake_Arm
 */
void Shake_Done(void)
{
}

/* Shake_Start - starts the standard screen shake: 6 px up, pulled back 2 px per frame.
 * In:  none
 * Out: g_ShakeVel = 6.0, g_ShakeGravity = 2.0; then Shake_Arm (it falls into it)
 * Evidence: move.l #$60000,g_ShakeVel / move.l #$20000,g_ShakeGravity; callers the a4 op $64A8,
 *           $240EE and 25 script uses ($137A0...). Falling into Shake_Arm: dk::jump (the same flow
 *           as falling in, no return address), so Shake_Arm's flags are this entry's
 */
void Shake_Start(void)
{
    g_ShakeVel = 0x60000u;
    g_ShakeGravity = 0x20000u;
    dk::jump(at_Shake_Arm);
}

/* Shake_StartLong - native callback: starts a 6 px shake with the weak pull 1.5 (it rings longer),
 * over a running one too.
 * In:  none
 * Out: g_ShakeVel = 6.0, g_ShakeGravity = 1.5; then Shake_Arm
 * Evidence: move.l #$60000,g_ShakeVel / move.l #$18000,g_ShakeGravity / bra.s Shake_Arm; a script's
 *           native call names $95D4 itself (the corpus's call-site log: 578 calls)
 */
void Shake_StartLong(void)
{
    g_ShakeVel = 0x60000u;
    g_ShakeGravity = 0x18000u;
    dk::jump(at_Shake_Arm);
}

/* Shake_StartLongIfIdle - native callback: Shake_StartLong unless a shake runs.
 * In:  none
 * Out: a shake running: the tst.l flags (Z clear), nothing changed; else Shake_StartLong's
 * Evidence: tst.l g_ShakeY / bne.s Shake_Done, falling into Shake_StartLong; script use $488F2.
 *           Prototype: &ccr added (the early exit's flags)
 */
void Shake_StartLongIfIdle(dk::Flags *ccr)
{
    *ccr = dk::Flags::test(g_ShakeY, 4, ccr->x);
    if (g_ShakeY != 0)
        return;
    Shake_StartLong();
}

/* Shake_StartBigIfIdle - native callback: starts the big shake (Shake_StartBig, which it falls
 * into) unless a shake runs.
 * In:  none
 * Out: a shake running: the tst.l flags (Z clear); else Shake_StartBig's
 * Evidence: tst.l g_ShakeY / bne.s Shake_Done, falling into Shake_StartBig (the next file); script
 *           uses $48810, $48828. Prototype: &ccr added (the early exit's flags)
 */
void Shake_StartBigIfIdle(dk::Flags *ccr)
{
    *ccr = dk::Flags::test(g_ShakeY, 4, ccr->x);
    if (g_ShakeY != 0)
        return;
    dk::jump(at_Shake_StartBig);
}
