#include "game.h"

/* TextMap_CmdBlinkGoto - text-map command $108: goes to one of two offsets by the 16-frame blink
 * phase (bit 4 of the frame counter).
 * In:  list = the offset for the phase off, then the one for it on
 * Out: then TextMap_CmdGoto on the chosen offset
 * Evidence: btst #4,(g_FrameCounter+3).w / beq TextMap_CmdGoto / addq.l #2,a0 / bra; computed entry
 *           in scenario-continue
 */
void TextMap_CmdBlinkGoto(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2,
                          uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    if (g_FrameCounter & 0x10u)
        *list += 2;
    dk::jump(at_TextMap_CmdGoto);
}

/* TextMap_CmdTypewriter - text-map command $118: the rest of the list is typed, one character
 * each time the speed accumulator carries (TextMap_ResumeTyping on the next calls).
 * In:  list = the speed word
 * Out: g_TextMapTyping = $FF; g_TextMapTypeSpeed = g_TextMapTypeAcc = the speed (also d0's low
 *      word); then TextMap_NextCmd
 * Evidence: st.b (g_TextMapTyping).w / move.w (a0)+,d0 / move.w d0,$F3BC / move.w d0,$F3BA / bra
 *           TextMap_NextCmd; computed entry in scenario-ending
 */
void TextMap_CmdTypewriter(uint32_t *d0, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1, uint32_t *a2,
                           uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    g_TextMapTyping = 0xFF;
    uint16_t speed = *(const u16 *)*list;
    *list += 2;
    *d0 = (*d0 & 0xFFFF0000u) | speed;
    g_TextMapTypeSpeed = speed;
    g_TextMapTypeAcc = speed;
    dk::jump(at_TextMap_NextCmd);
}
