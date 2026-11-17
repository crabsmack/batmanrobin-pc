#include "game.h"

/* Player_InitRecords - resets both player records for a new game or a continue: which one is in
 * play (g_StartPad: the pad that pressed Start), the Robin flag for the character player 1 did not
 * pick, the pad numbers, then Player_InitRecord's defaults for each.
 * In:  g_StartPad, g_P1IsRobin
 * Out: g_Player1 / g_Player2: pad 1 / 2; flags: the record of the pad that pressed Start in play,
 *      player 2's HUD_RIGHT-side bit ($84 / $85: panel text + HUD right), Robin on player 1 when
 *      g_P1IsRobin, else on player 2; then both records' defaults (Player_InitRecord); a1 =
 *      g_Player2, and a0 = g_Player2 for the second Player_InitRecord, which returns to the caller
 * Evidence: move.b #1,d0 / move.b #$84,d1 / cmpi.b #2,g_StartPad / bne: move.b #0,d0 / move.b
 *           #$85,d1; tst.b g_P1IsRobin / beq: ori.b #8,d0 / bra, else ori.b #8,d1; the pad and
 *           flag stores; bsr Player_InitRecord / movea.l a1,a0 / bra.w Player_InitRecord (a jump:
 *           its rts is this function's, so a0 goes out as an output); callers Main_RunSection's
 *           continue path ($9EFC) and the title flow ($9F36)
 */
uint32_t Player_InitRecords(uint32_t *a0)
{
    uint8_t flags1 = PLAYER_IN_PLAY;
    uint8_t flags2 = PLAYER_PANEL_TEXT | PLAYER_HUD_RIGHT;
    if (g_StartPad == 2) {
        flags1 = 0;
        flags2 |= PLAYER_IN_PLAY;
    }
    if (g_P1IsRobin)
        flags1 |= PLAYER_ROBIN;
    else
        flags2 |= PLAYER_ROBIN;
    PlayerRec *p1 = (PlayerRec *)g_Player1;
    PlayerRec *p2 = (PlayerRec *)g_Player2;
    p1->pad = 1;
    p1->flags = flags1;
    p2->pad = 2;
    p2->flags = flags2;
    Player_InitRecord(dk::addr(p1));
    *a0 = dk::addr(p2);
    dk::jump(at_Player_InitRecord);
    return dk::addr(p2);
}
