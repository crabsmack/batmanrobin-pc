#include "game.h"

/* Frame_BeginStageHook - an empty per-frame hook (a bare rts) at the end of Frame_BeginStage;
 * next to the debug pause code, likely a stripped debug hook.
 * In:  none
 * Out: none
 * Evidence: the only instruction is rts; only caller Frame_BeginStage ($9B86); the 38 bytes after
 *           it ($15676-$1569B) are never read
 */
void Frame_BeginStageHook(void)
{
}
