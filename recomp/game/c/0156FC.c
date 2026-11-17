#include "game.h"

/* Debug_PlacePlayerAtCursor - moves a player in play to the free-camera cursor (the debug pause
 * menu does it for both players when the free camera turns off).
 * In:  player = the player record (g_Player1 / g_Player2)
 * Out: in play: player = its object, whose x/y = the cursor's integer x/y
 * Evidence: btst #0,$25(a1) / beq / movea.w $2C(a1),a1 / move.w $F41A,$3E(a1) / move.w $F41E,$42(a1);
 *           static only (the debug menu never ran)
 */
void Debug_PlacePlayerAtCursor(uint32_t *player)
{
    PTR(const PlayerRec) rec;
    rec.v = *player;
    if (!(rec->flags & PLAYER_IN_PLAY))
        return;
    Vm2Obj *obj = dk::handle<Vm2Obj>(rec->object);
    *player = dk::addr(obj);
    obj->x = (int16_t)(g_DebugCursorX >> 16);
    obj->y = (int16_t)(g_DebugCursorY >> 16);
}

/* Debug_CentreCursor - puts the free-camera cursor at the screen's centre (the debug pause menu,
 * when the free camera turns on).
 * In:  g_CameraX / g_CameraY
 * Out: the cursor's integer x/y = the camera's + $A0 / + $70 (the fractions kept)
 * Evidence: move.w (g_CameraX).w,d0 / addi.w #$A0,d0 / move.w d0,(g_DebugCursorX).w, the same for y
 *           with $70; branch from Debug_PauseMenu; static only
 */
void Debug_CentreCursor(void)
{
    uint16_t x = (uint16_t)((g_CameraX >> 16) + 0xA0);
    uint16_t y = (uint16_t)((g_CameraY >> 16) + 0x70);
    g_DebugCursorX = ((uint32_t)x << 16) | (g_DebugCursorX & 0xFFFFu);
    g_DebugCursorY = ((uint32_t)y << 16) | (g_DebugCursorY & 0xFFFFu);
}
