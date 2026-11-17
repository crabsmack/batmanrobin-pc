#include "game.h"

/* The a6 VM's buffer ops ($0070E0-$007185): natives, sprite banks, LZ decompression (now or as
 * the background task), the buffer-pointer slots and palettes. Each continues with VM6_Next (or
 * VM6_Reload when its callee clobbers a6), or parks the script for Frame_End. */

/* Op6_CallNative - a6 op: calls a native routine (dc.l) with a6 at its inline arguments, then
 * continues after them.
 * In:  a6 = script: long routine, then the routine's own inline words
 * Out: the routine's d0-d2 and a6 (past its arguments); VM6_Next
 * Evidence: movea.l (a6)+,a0 / jsr (a0) / bra VM6_Next; `70E0 0000 8580 000D` ($1FB30,
 *           ArtCache_AddBlocksInline), `70E0 0000 772E 2300` ($33050, Section_SetIdInline); TAS
 *           f4539-77148. Prototype: d0-d2 widened to longs (the routines' registers pass through)
 */
void Op6_CallNative(uint32_t *d0, uint32_t *d1, uint32_t *d2, const u16 **script)
{
    uint32_t routine = vm6_long(script);
    Op6NativeFn(routine, d0, d1, d2, script);
    dk::jump(at_VM6_Next);
}

/* Op6_InitBankFrames - a6 op: builds the RAM sprite-frame entries of a sprite bank already in a
 * scene buffer (Spr_InitBankFrames, handle base $8002).
 * In:  a6 = script: word g_BufPtrs offset (the bank), word frame-table offset (from $A764)
 * Out: d0's low word = the frame-table offset (as the callee leaves it), d1 = $8002,
 *      g_A6BankHandleBase = d1; VM6_Next
 * Evidence: move.w #$8002,(g_A6BankHandleBase).w / buffer slot / move.w (a6)+,d0 /
 *           jsr Spr_InitBankFrames / move.w d1,(g_A6BankHandleBase).w; 12 uses, e.g. $1FB26
 *           `70E6 0008 FC40` after `7120 0008 0006 E042`; 16 TAS frames
 */
void Op6_InitBankFrames(uint32_t *d0, uint16_t *d1, uint16_t *d7, uint32_t *a1, uint32_t *a2, const u16 **script)
{
    g_A6BankHandleBase = 0x8002;
    uint32_t bank = vm6_slot(vm6_word(script))->raw();
    *d0 = (*d0 & 0xFFFF0000u) | vm6_word(script);
    *d1 = g_A6BankHandleBase;
    Spr_InitBankFrames(d0, *d1, d7, bank, a1, a2);
    g_A6BankHandleBase = *d1;
    dk::jump(at_VM6_Next);
}

/* Op6_Decompress - a6 op: decompresses an LZ block into a scene buffer now, then continues.
 * In:  a6 = script: word g_BufPtrs offset (the destination), long LZ block
 * Out: the buffer filled, g_LzOutEnd = the output's end; g_A6ScriptPc after the operands, then
 *      VM6_Reload (a6 reloaded from it)
 * Evidence: lea g_BufPtrs,a1 / adda.w (a6)+,a1 / movea.l (a1),a1 / movea.l (a6)+,a0 /
 *           move.l a6,(g_A6ScriptPc).w / jsr Lz_Decompress / bra VM6_Reload; 61 uses, e.g.
 *           Tbl_01FAB2 `7108 0000 0005 E63C`; 24 corpus runs
 */
void Op6_Decompress(uint32_t *d1, uint32_t *d2, uint32_t *d6, uint32_t *d7, uint32_t *a1, uint32_t *a2,
                    uint32_t *a3, const u16 **script)
{
    *a1 = vm6_slot(vm6_word(script))->raw();
    uint32_t block = vm6_long(script);
    g_A6ScriptPc = ADDR68(*script);
    dk::Flags lz_flags{};                    /* the decompressor sets every flag before reading one */
    Lz_Decompress(d1, d2, d6, d7, &block, a1, a2, a3, &lz_flags);
    dk::jump(at_VM6_Reload);
}

/* Op6_DecompressBg - a6 op: decompresses an LZ block into a scene buffer as the background task
 * and parks the script until it is done (ruling 1: the task is BgTask_Start's).
 * In:  a6 = script: word g_BufPtrs offset (the destination), long LZ block
 * Out: a1 = the buffer, a0 = the block, g_A6ScriptPc after them, g_A6ResumeHook =
 *      VM6_WaitBgDecompress; jumps to BgTask_Start (which returns to the script's runner)
 * Evidence: lea g_BufPtrs,a1 / ... / move.l #$713E,(g_A6ResumeHook).w / jmp BgTask_Start; 14
 *           uses, e.g. $1FB1E `7120 0008 0006 E042`; 10 corpus runs
 */
void Op6_DecompressBg(uint32_t *d0, uint32_t d1, uint32_t *a0, uint32_t *a1, uint32_t a3, uint32_t *a4,
                      const u16 **script)
{
    (void)d0, (void)d1, (void)a3, (void)a4;
    *a1 = vm6_slot(vm6_word(script))->raw();
    *a0 = vm6_long(script);
    g_A6ScriptPc = ADDR68(*script);
    g_A6ResumeHook = at_VM6_WaitBgDecompress;
    dk::jump(at_BgTask_Start);
}

/* VM6_WaitBgDecompress - a6 resume hook of Op6_DecompressBg: waits (returns) while the background
 * decompressor runs, then resumes the script.
 * In:  g_BgTaskActive (cleared by the decompressor when it ends)
 * Out: returns, or VM6_Resume
 * Evidence: tst.b (g_BgTaskActive).w / beq VM6_Resume / rts; hook value $713E; 10 corpus runs
 */
void VM6_WaitBgDecompress(uint32_t *a6)
{
    (void)a6;
    if (g_BgTaskActive == 0)
        dk::jump(at_VM6_Resume);
}

/* Op6_StoreLzEnd - a6 op: stores the end of the last decompression into a g_BufPtrs slot (the next
 * block can be unpacked right after it).
 * In:  a6 = script: word g_BufPtrs offset
 * Out: the slot = g_LzOutEnd; VM6_Next
 * Evidence: adda.w (a6)+,a0 / move.l (g_LzOutEnd).w,(a0) / bra VM6_Next; 6 uses, e.g. $1FB2C
 *           `7148 000C`; 12 corpus runs
 */
void Op6_StoreLzEnd(const u16 **script)
{
    vm6_slot(vm6_word(script))->v = g_LzOutEnd;
    dk::jump(at_VM6_Next);
}

/* Op6_SetBufPtrToEntry - a6 op: points one g_BufPtrs slot at an entry of another slot's buffer.
 * In:  a6 = script: word source slot offset, word entry offset, word destination slot offset
 * Out: g_BufPtrs[dest] = the entry; a1 = the destination slot; VM6_Next
 * Evidence: lea g_BufPtrs,a0 / movea.l a0,a1 / adda.w (a6)+,a0 / movea.l (a0),a0 / adda.w (a6)+,a0
 *           / adda.l (a0),a0 / adda.w (a6)+,a1 / move.l a0,(a1); 11 uses, e.g. $1FC3E
 *           `7158 0000 0008 0004`; 15 corpus runs
 */
void Op6_SetBufPtrToEntry(uint32_t *a1, const u16 **script)
{
    uint32_t buf = vm6_slot(vm6_word(script))->raw();
    uint32_t entry = vm6_entry(buf, vm6_word(script));
    PTR(u8) *dest = vm6_slot(vm6_word(script));
    dest->v = entry;
    *a1 = ADDR68(dest);
    dk::jump(at_VM6_Next);
}

/* Op6_LoadPalettesYield - a6 op: loads palettes from a buffer entry into the lines a mask picks
 * (VM6_LoadPalettes), then parks the script until the next frame end.
 * In:  a6 = script: word g_BufPtrs offset, word entry offset, byte count-1, byte line mask
 * Out: g_A6ScriptPc after the operands, g_A6ResumeHook = VM6_Resume; the condition codes of that
 *      move.l (all clear)
 * Evidence: bsr VM6_LoadPalettes / move.l a6,(g_A6ScriptPc).w / move.l #$70A8,(g_A6ResumeHook).w /
 *           rts; no use in the decoded scripts, ran in 6 corpus runs
 */
void Op6_LoadPalettesYield(uint8_t *d0, uint32_t *d7, uint32_t *a0, uint32_t *a1, const u16 **script,
                           dk::Flags *ccr)
{
    VM6_LoadPalettes(d0, d7, a0, a1, script);
    g_A6ScriptPc = ADDR68(*script);
    g_A6ResumeHook = at_VM6_Resume;
    *ccr = dk::Flags{ ccr->x, false, false, false, false };
}

/* Op6_LoadPalettes - a6 op: loads palettes from a buffer entry into the lines a mask picks and
 * continues: pushes VM6_Next as the return address and runs into VM6_LoadPalettes (the next file),
 * whose rts lands on NEXT.
 * In:  a6 = script: word g_BufPtrs offset, word entry offset, byte count-1, byte line mask
 * Out: as VM6_LoadPalettes, then VM6_Next
 * Evidence: pea VM6_Next(pc) falling into VM6_LoadPalettes; 32 uses, e.g. $1FC36
 *           `7182 0000 0004 0040` (one palette to line 1); 24 corpus runs
 */
void Op6_LoadPalettes(uint8_t *d0, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a6)
{
    (void)d0, (void)d7, (void)a0, (void)a1, (void)a6;
    dk::push_long(at_VM6_Next);
    dk::jump(at_VM6_LoadPalettes);
}
