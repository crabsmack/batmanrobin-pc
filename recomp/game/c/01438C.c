#include "game.h"

/* The text-map interpreter's entry ($01438C-$0143A9). Its state lives in registers across the
 * commands, which jump to each other (each command a file of its own): a0 the command list, a5
 * the font map, a6 the write pointer into the plane image, d6 the tile base, d7 a character count.
 * a1-a6 of the caller are saved on the 68000's stack until TextMap_CmdEnd (or a typewriter exit)
 * pops them, so every transfer here is a jump, never a call. */

/* TextMap_Run - interprets a text-map command list that writes font tiles into the 40x28-word
 * plane image g_SceneWorkBuf, or, in typewriter mode, writes the next character of the saved
 * text run when one is due.
 * In:  list = the command list (words: the command's offset from TextMap_CmdEnd, its operands);
 *      g_TextMapFont / g_TextMapTileBase (TextMap_LoadFont); g_TextMapTyping
 * Out: a1-a6 pushed; font = g_TextMapFont, dst = the plane image, base = the tile base; then
 *      TextMap_ResumeTyping (typing) or the first command (TextMap_NextCmd)
 * Evidence: movem.l a1-a6,-(a7) / movea.l (g_TextMapFont).w,a5 / lea (g_SceneWorkBuf).l,a6 / move.w
 *           (g_TextMapTileBase).w,d6 / tst.b (g_TextMapTyping).w / bne TextMap_ResumeTyping; callers
 *           the copyright screen ($49FA0, TAS f164), the title menu ($499B6), options ($499E8),
 *           $49212 and the ending texts ($4A5E8, $4A734)
 */
void TextMap_Run(uint32_t *d0, uint16_t *base, uint16_t *d7, uint32_t *list, uint32_t *a1, uint32_t *a2, uint32_t *a3,
                 uint32_t *a4, uint32_t *font, uint32_t *dst)
{
    dk::push_long(*dst);                     /* movem.l a1-a6,-(a7) */
    dk::push_long(*font);
    dk::push_long(*a4);
    dk::push_long(*a3);
    dk::push_long(*a2);
    dk::push_long(*a1);
    *font = g_TextMapFont;
    *dst = dk::addr(g_SceneWorkBuf);
    *base = g_TextMapTileBase;
    if (g_TextMapTyping) {
        dk::jump(at_TextMap_ResumeTyping);
        return;
    }
    dk::jump(at_TextMap_NextCmd);            /* falls into it: a jump keeps the stack as the asm's */
}

/* TextMap_NextCmd - the interpreter's dispatch: the list's next word is the command's offset from
 * TextMap_CmdEnd, jumped to.
 * In:  list = at the command word
 * Out: list past it; d0 low word = the offset (move.w: the condition codes its tst); the command
 * Evidence: move.w (a0)+,d0 / jmp TextMap_CmdEnd(pc,d0.w); offsets 0 End, $E Goto, $12 Return, $22
 *           SetPos, $58 Text, $96/$A0 Number, $EA MenuItem, $108 BlinkGoto, $118 Typewriter (NOTES
 *           Text-map menus); the tail of every command
 */
void TextMap_NextCmd(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2,
                     uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6, dk::Flags *ccr)
{
    uint16_t offset = *(const u16 *)*list;
    *list += 2;
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    *ccr = dk::Flags::test(offset, 2, ccr->x);
    dk::jump(at_TextMap_CmdEnd + (uint32_t)(int32_t)(int16_t)offset);
}
