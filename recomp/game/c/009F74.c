#include "game.h"

/* The stage openings ($009F74-$009FBB), sections $0001-$0004: each starts its stage's music and
 * runs its scene list (the shared stage opening $4A180, then the stage's title card) through
 * Scene_RunList, which returns to the section loop. a0 (the list) is an output of the
 * prototypes, so the bra to Scene_RunList hands it on (the measured contract called it a
 * clobber: Scene_RunList reads it). */

static constexpr uint32_t Tbl_00A166 = 0x00A166u;   /* stage 1: $4A180, $4A216 */
static constexpr uint32_t Tbl_00A172 = 0x00A172u;   /* stage 2: $4A180, $4A2DC */
static constexpr uint32_t Tbl_00A17E = 0x00A17Eu;   /* stage 3: $4A180, $4A3A4 */
static constexpr uint32_t Tbl_00A18A = 0x00A18Au;   /* stage 4: $4A180, $4A464 */

/* the shared body: move.w #music,d0 / bsr Sound_PlayMusic / lea list,a0 (the bra Scene_RunList
   follows in each entry) */
static void stage_intro(uint16_t music, uint32_t scene_list, uint32_t *d1, uint32_t *list, uint32_t *a1,
                        uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    uint32_t d0 = music, d7 = 0;   /* d0's high word and d7 are not inputs: Sound_PlayMusic sets them */
    Sound_PlayMusic(&d0, d1, &d7, *a1, *a2, *a3, *a4, *a5, *a6);
    *list = scene_list;
}

/* Stage_Intro1 - section $0001: stage 1's opening, music 8 and the scene list Tbl_00A166.
 * In:  the scene registers (handed on)
 * Out: a0 = Tbl_00A166; Sound_PlayMusic's d1; then as Scene_RunList
 * Evidence: move.w #8,d0 / bsr Sound_PlayMusic / lea Tbl_00A166,a0 / bra Scene_RunList; Tbl_00A0AC
 *           maps section $0001 here; TAS f477
 */
void Stage_Intro1(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list,
                  uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d3; (void)d4; (void)d5; (void)d6;
    stage_intro(8, Tbl_00A166, d1, list, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Scene_RunList);
}

/* Stage_Intro2 - section $0002: stage 2's opening, music $C and the scene list Tbl_00A172.
 * In:  the scene registers (handed on)
 * Out: a0 = Tbl_00A172; then as Scene_RunList
 * Evidence: move.w #$C,d0 / bsr Sound_PlayMusic / lea Tbl_00A172,a0 / bra Scene_RunList; follows
 *           section $1400 in Tbl_009FFC
 */
void Stage_Intro2(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list,
                  uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d3; (void)d4; (void)d5; (void)d6;
    stage_intro(0x0C, Tbl_00A172, d1, list, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Scene_RunList);
}

/* Stage_Intro3 - section $0003: stage 3's opening, music $E and the scene list Tbl_00A17E.
 * In:  the scene registers (handed on)
 * Out: a0 = Tbl_00A17E; then as Scene_RunList
 * Evidence: move.w #$E,d0 / bsr Sound_PlayMusic / lea Tbl_00A17E,a0 / bra Scene_RunList; follows
 *           section $2401 in Tbl_009FFC
 */
void Stage_Intro3(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list,
                  uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d3; (void)d4; (void)d5; (void)d6;
    stage_intro(0x0E, Tbl_00A17E, d1, list, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Scene_RunList);
}

/* Stage_Intro4 - section $0004: stage 4's opening, music $B and the scene list Tbl_00A18A.
 * In:  the scene registers (handed on)
 * Out: a0 = Tbl_00A18A; then as Scene_RunList
 * Evidence: move.w #$B,d0 / bsr Sound_PlayMusic / lea Tbl_00A18A,a0 / bra Scene_RunList; follows
 *           section $3401 in Tbl_009FFC
 */
void Stage_Intro4(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *list,
                  uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6)
{
    (void)d3; (void)d4; (void)d5; (void)d6;
    stage_intro(0x0B, Tbl_00A18A, d1, list, a1, a2, a3, a4, a5, a6);
    dk::jump(at_Scene_RunList);
}
