#include "game.h"

/* the shaft's depth bands (ROM): word bands - 1, then per band word columns - 1 and a divisor */
static constexpr uint32_t Tbl_024642 = 0x024642u;

static constexpr uint16_t VDP_PLANEB_AT_C000 = 0x8406;     /* reg 4: plane B's name table at $C000 */
static constexpr uint32_t VDP_HINT_NEVER_VSCROLL_COLS = 0x8AFF8F04u;   /* reg $A = $FF, reg $F = 4 */
static constexpr uint32_t VSRAM_WRITE_2 = 0x40020010u;     /* VSRAM 2: plane B's first column */
enum { ARENA_SPLIT_COLUMNS = 20 };

/* Stage2_ArenaHIntSplit - H-int at the arena's split line: plane B takes plane A's name table
 * ($C000) and its 20 column V-scrolls from the split table, so below the line plane B shows the
 * other side of the tilting floor
 * In:  H-int (via the jmp stub at g_HIntCode); a0/a1 = the interrupted code's (only saved)
 * Out: VDP reg 4 = 6, reg $A = $FF, reg $F = 4, VSRAM 2.. = g_Stage2Vars->col_b_split; rte
 * Evidence: move.l a0,-(a7) / lea $C00004,a0 / move.w #$8406 / move.l #$8AFF8F04 / move.l
 *           #$40020010 / subq.w #4,a0 / move.l a1,-(a7) / lea g_SceneVars+$82,a1 / 10 x move.l
 *           (a1)+,(a0) / rte; jmp stub from Stage2_ArenaVIntUpload ($0245C0); TAS f32067-f35416.
 *           The prototype takes a0/a1 as inputs (only pushed: the stack image the asm leaves)
 */
void Stage2_ArenaHIntSplit(uint32_t a0, uint32_t a1)
{
    dk::push_long(a0);
    hw::port_write16(hw::VDP_CTRL, VDP_PLANEB_AT_C000);
    hw::port_write32(hw::VDP_CTRL, VDP_HINT_NEVER_VSCROLL_COLS);
    hw::port_write32(hw::VDP_CTRL, VSRAM_WRITE_2);
    dk::push_long(a1);
    const s16 *col = g_Stage2Vars->col_b_split;
    for (int i = 0; i < ARENA_SPLIT_COLUMNS; i += 2)   /* move.l: two columns a write */
        hw::port_write32(hw::VDP_DATA, ((uint32_t)(uint16_t)col[i] << 16) | (uint16_t)col[i + 1]);
    (void)dk::pop_long();
    (void)dk::pop_long();
    dk::rte();
}

/* divs.w: the 32-bit dividend by a signed word; the remainder in the high word, the quotient in
   the low. A quotient that doesn't fit a word sets V and leaves the dividend unchanged. */
static uint32_t Lift_Divs(uint32_t dividend, int16_t divisor)
{
    int64_t q = (int64_t)(int32_t)dividend / divisor;
    if (q < -32768 || q > 32767)
        return dividend;
    int32_t r = (int32_t)dividend % divisor;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

/* Stage2_LiftColumnScroll - the shaft's 20 plane A column V-scrolls from the view y in depth
 * bands (Tbl_024642), giving the girders and walls their parallax
 * In:  g_Stage2ViewY (16.16)
 * Out: g_Stage2Vars->col_a[] = (view y >> 8) / the band's divisor per column; d1 = the last
 *      band's divs.w result (remainder:quotient), d6.w = $FFFF (dbra)
 * Evidence: move.l g_Stage2ViewY,d0 / asr.l #8 / lea Tbl_024642,a0 / lea g_SceneVars+$A,a1 /
 *           move.w (a0)+,d7 / per band: move.w (a0)+,d6 / move.l d0,d1 / divs.w (a0)+,d1 / move.w
 *           d1,(a1)+ x d6+1; the lift branch of Stage2_UpdateScroll (bra); TAS f21158-f32065
 */
void Stage2_LiftColumnScroll(uint32_t *d1, uint16_t *d6)
{
    int32_t depth = (int32_t)(uint32_t)g_Stage2ViewY >> 8;   /* asr.l */
    PTR(const s16) tbl;
    tbl.v = Tbl_024642;
    s16 *col = g_Stage2Vars->col_a;
    int16_t bands = *tbl++;
    do {
        uint16_t cols = (uint16_t)*tbl++;
        *d1 = Lift_Divs((uint32_t)depth, *tbl++);
        do {
            *col++ = (int16_t)(uint16_t)*d1;
        } while (cols-- != 0);   /* dbra d6 */
        *d6 = cols;
    } while (bands-- != 0);      /* dbra d7 */
}
