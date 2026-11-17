#include "game.h"

/* The a4 VM's NEXT and its native-code doors ($0053BA-$0053CB). An a4 script is a list of op
 * handler words, each followed by its operands; every op ends by fetching the next handler word
 * (movea.w (a4)+,a0) and jumping to it, until an op returns (rts) to whoever started the run:
 * the object runner (VM4_RunObject), Fx_RunScript, a native caller or Op4_Gosub. */

/* the handler a script word names (sign-extended), and the script past it */
static uint32_t fetch_op(const u16 **script)
{
    return dk::code((int16_t)(uint16_t)*(*script)++);
}

/* VM4_Next - the a4 VM's NEXT: jumps to the handler the script's next word names; also the entry
 * of native code that has set a4/a6 (and a5) itself.
 * In:  a4 = script (at a handler word); a6 = the object (or g_SpawnCtx); a5 = child or a6; every
 *      other register as the op or native left it for the next op
 * Out: a4 past the word; a0 = the handler, jumped to (every register goes on to it)
 * Evidence: movea.w (a4)+,a0 / jmp (a0), the tail of every a4 op; bsr/jsr from Fx_RunScript $91F2,
 *           Fx_RunScriptNoParent, $915C, Weapon_Fire $10A7E; 99,175 TAS frames
 */
void VM4_Next(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6,
              uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5,
              uint32_t *a6)
{
    (void)d0, (void)d1, (void)d2, (void)d3, (void)d4, (void)d5, (void)d6, (void)d7;
    (void)a1, (void)a2, (void)a3, (void)a5, (void)a6;
    PTR(const u16) script;
    script.v = *a4;
    *a0 = dk::code((int16_t)(uint16_t)*script++);
    *a4 = script.raw();
    dk::jump(*a0);
}

/* Op4_SkipWord - a4 VM: skips one script word (a branch offset not taken), then NEXT.
 * In:  script = the word to skip
 * Out: script past it and the next handler word; that op, jumped to
 * Evidence: addq.l #2,a4 then NEXT; the not-taken path of Op4_TimerLoop and the op at $607C (no
 *           script uses it as an op)
 */
void Op4_SkipWord(const u16 **script)
{
    (*script)++;
    dk::jump(fetch_op(script));
}

/* Op4_Native - a4 op: leaves the script and runs the 68000 code right after the op word; that
 * code goes back to script with `jsr VM4_Enter` (or jumps to an op such as Op4_Kill).
 * In:  code = a4, the native code's address
 * Out: jumped to it
 * Evidence: jmp (a4); script uses $3D8FE / $3D912 followed by 68000 code ending in jsr ($53C6).w or
 *           jmp Op4_Kill; 4 corpus runs incl. the TAS
 */
void Op4_Native(uint32_t code)
{
    dk::jump(code);
}

/* VM4_Enter - enters a4 script from native code: the return address of the `jsr VM4_Enter` is the
 * script (the bytes after the jsr), popped as a4, then NEXT.
 * In:  the return address = the script; the running object's a5/a6
 * Out: a4 = the script past its first handler word; that op, jumped to; never returns to the jsr
 * Evidence: movea.l (a7)+,a4 / NEXT; 2 jsr ($53C6).w sites, $3D92E and $42922, each ending a
 *           native fragment entered by Op4_Native; dkit.toml [disasm] noreturn
 */
uint32_t VM4_Enter(void)
{
    const u16 *script = dk::return_data<u16>();
    dk::drop_return();
    uint32_t handler = fetch_op(&script);
    dk::jump(handler);
    return dk::addr(script);
}
