#include "game.h"

/* The script-flow ops ($002CE6-$002D27): gosub with arguments (absolute or relative; entered at
 * any of the argument stores, so 4, 3 or 2 words), gosub, goto absolute, and the shared
 * branch-taken tails Op2_SkipGoto / Op2_Goto. A gosub pushes the script's return point on the
 * 68000 stack, where Op2_Return pops it. */

/* NEXT: the handler the script's next word names, jumped to */
static void next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* an argument word goes in the high word of its g_VM2ArgN long (move.w to its address) */
static uint32_t with_high_word(uint32_t arg, uint16_t w)
{
    return (arg & 0xFFFFu) | ((uint32_t)w << 16);
}

/* Op2_GosubAbsArgs - a2 op: calls an absolute script subroutine with four word arguments:
 * stores them in the high words of g_VM2Arg3, Arg2, Arg1, Arg0, pushes the return point (after
 * the long) and continues at the subroutine.
 * In:  script = words arg3, arg2, arg1, arg0, long script address
 * Out: g_VM2Arg3..0 high words; (sp) = the return script; script = the subroutine; NEXT
 * Evidence: move.w (a2)+ to $DEDA/$DED6/$DED2/$DECE / pea 4(a2) / movea.l (a2),a2 / NEXT; 6 runs;
 *           $2DFA6 `2CE6 FFF1 000F 00F0 00DC 00027D50`
 */
void Op2_GosubAbsArgs(const u16 **script)
{
    g_VM2Arg3 = with_high_word(g_VM2Arg3, *(*script)++);
    g_VM2Arg2 = with_high_word(g_VM2Arg2, *(*script)++);
    g_VM2Arg1 = with_high_word(g_VM2Arg1, *(*script)++);
    g_VM2Arg0 = with_high_word(g_VM2Arg0, *(*script)++);
    dk::push_long(dk::addr(*script) + 4);
    PTR(const u16) target;
    target.v = *(const u32 *)*script;
    *script = target;
    uint32_t handler;
    next(&handler, script);
}

/* Op2_GosubArgs - a2 op: calls a relative script subroutine with four word arguments (the high
 * words of g_VM2Arg3..Arg0), then as Op2_Gosub.
 * In:  script = words arg3, arg2, arg1, arg0, word offset
 * Out: g_VM2Arg3..0 high words; (sp) = the return script; script = the subroutine; NEXT
 * Evidence: the four argument stores of Op2_GosubAbsArgs, falling into Op2_Gosub; 20 runs
 */
void Op2_GosubArgs(const u16 **script)
{
    g_VM2Arg3 = with_high_word(g_VM2Arg3, *(*script)++);
    Op2_GosubArgs3(script);
}

/* Op2_GosubArgs3 - a2 op (Op2_GosubArgs past its first store): a relative gosub with three word
 * arguments (g_VM2Arg2, Arg1, Arg0; Arg3 kept).
 * In:  script = words arg2, arg1, arg0, word offset
 * Out: g_VM2Arg2..0 high words; then as Op2_Gosub
 * Evidence: script handler words $2D04 from NEXT sites (Op2_Return's $2D2E 1,562, $3B3A 386,
 *           $0EA4 279, ...): 2,516 call-site log entries over the corpus
 */
void Op2_GosubArgs3(const u16 **script)
{
    g_VM2Arg2 = with_high_word(g_VM2Arg2, *(*script)++);
    Op2_GosubArgs2(script);
}

/* Op2_GosubArgs2 - a2 op (Op2_GosubArgs past two stores): a relative gosub with two word arguments
 * (g_VM2Arg1, Arg0; Arg3/Arg2 kept).
 * In:  script = words arg1, arg0, word offset
 * Out: g_VM2Arg1/0 high words; then as Op2_Gosub
 * Evidence: script handler words $2D08 from NEXT sites ($1BBC 240, ...): 277 call-site log
 *           entries over the corpus
 */
void Op2_GosubArgs2(const u16 **script)
{
    g_VM2Arg1 = with_high_word(g_VM2Arg1, *(*script)++);
    g_VM2Arg0 = with_high_word(g_VM2Arg0, *(*script)++);
    Op2_Gosub(script);
}

/* Op2_Gosub - a2 op: calls a relative script subroutine: pushes the script after the offset word
 * and continues at the offset word's address plus its value; Op2_Return pops it.
 * In:  script = word offset
 * Out: (sp) = the return script; script = the subroutine; NEXT
 * Evidence: pea $2(a2) / adda.w (a2),a2 / NEXT; a computed-jump target in 146 of 153 runs; 391
 *           script words $2D10, e.g. $18242 `2D10 FF08`
 */
void Op2_Gosub(const u16 **script)
{
    dk::push_long(dk::addr(*script) + 2);
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    uint32_t handler;
    next(&handler, script);
}

/* Op2_GotoAbs - a2 op: continues the script at an absolute address.
 * In:  script = long address
 * Out: script = the target; NEXT
 * Evidence: movea.l (a2),a2 / NEXT; 12 runs; $2DF8E `2D1A 00027CD8`
 */
void Op2_GotoAbs(const u16 **script)
{
    PTR(const u16) target;
    target.v = *(const u32 *)*script;
    *script = target;
    uint32_t handler;
    next(&handler, script);
}

/* Op2_Goto - a2 op and the shared branch-taken tail: the script continues at the offset word's
 * address plus its value.
 * In:  script = word offset
 * Out: script = the target; handler = its first op's; NEXT
 * Evidence: adda.w (a2),a2 / NEXT; dispatched in 152 runs (1586 script words $2D22) and branched to
 *           by most conditional ops
 */
void Op2_Goto(uint32_t *handler, const u16 **script)
{
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    next(handler, script);
}

/* Op2_SkipGoto - a2 op tail: the branch-taken path of the ops with a word before the offset:
 * skips it, then as Op2_Goto.
 * In:  script = a word to skip, then the branch offset
 * Out: as Op2_Goto
 * Evidence: addq.l #2,a2, falling into Op2_Goto; the blt/bgt targets of Op2_BranchOffScrX and
 *           Op2_SwitchScrX
 */
void Op2_SkipGoto(uint32_t *handler, const u16 **script)
{
    (*script)++;
    Op2_Goto(handler, script);
}
