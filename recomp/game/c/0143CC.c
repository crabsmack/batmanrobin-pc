#include "game.h"

enum { TEXTMAP_ROW_BYTES = 0x28 };   /* the operand's row is in units of 2 rows: row*2 x 40 */

/* TextMap_CmdSetPos - text-map command $22: moves the write position to an offset of the 40-column
 * plane image given as two signed bytes, column*2 and row*2 (so the byte offset is x + y*$28).
 * In:  list = the two bytes
 * Out: d0 low word = the byte offset, d1 = y*$28 (mulu, a long); dst = g_SceneWorkBuf + offset;
 *      then TextMap_NextCmd
 * Evidence: move.b (a0)+,d0 / ext.w d0 / move.b (a0)+,d1 / ext.w d1 / mulu.w #$28,d1 / add.w d1,d0 /
 *           lea (g_SceneWorkBuf).l,a6 / adda.w d0,a6; Tbl_0498EA '0022 2006' puts 'OPTIONS' at
 *           column 16, row 3; 9 runs
 */
void TextMap_CmdSetPos(uint32_t *d0, uint32_t *d1, uint16_t *d6, uint16_t *d7, const u8 **list, uint32_t *a1,
                       uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *dst)
{
    uint16_t x = (uint16_t)(int16_t)(int8_t)*(*list)++;
    uint16_t y = (uint16_t)(int16_t)(int8_t)*(*list)++;
    *d1 = (uint32_t)y * TEXTMAP_ROW_BYTES;   /* mulu.w: the sign-extended word, unsigned */
    uint16_t offset = (uint16_t)(x + (uint16_t)*d1);
    *d0 = (*d0 & 0xFFFF0000u) | offset;
    *dst = dk::addr(g_SceneWorkBuf) + (uint32_t)(int32_t)(int16_t)offset;
    dk::jump(at_TextMap_NextCmd);
}
