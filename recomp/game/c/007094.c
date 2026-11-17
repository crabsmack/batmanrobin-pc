#include "game.h"

/* VM6_Start - starts an a6 loader script for Frame_End: stores its pc and installs VM6_Resume as
 * the resume hook, so the script runs from the next frame end. Unlike Op2_StartA6Script it leaves
 * g_A6ScriptBusy alone (VM6_Next sets it once the script runs).
 * In:  script (a6)
 * Out: g_A6ScriptPc = script, g_A6ResumeHook = VM6_Resume; d0 = 0 (moveq: Z set, the trigger
 *      callback's "done")
 * Evidence: move.l a6,(g_A6ScriptPc).w / move.l #$70A8,(g_A6ResumeHook).w / moveq #0,d0 / rts;
 *           callers VM6_StartFromTrigger and $33420 (Tbl_0330FC, stage 2 section setup); 3 corpus
 *           runs. Prototype: d0 (measured as a clobber) is the output whose tst gives the Z the
 *           trigger walker reads
 */
uint32_t VM6_Start(uint32_t script)
{
    g_A6ScriptPc = script;
    g_A6ResumeHook = at_VM6_Resume;
    return 0;
}
