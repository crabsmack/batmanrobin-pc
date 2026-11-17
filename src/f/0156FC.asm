; ---------------------------------------------------------------------------
; Debug_PlacePlayerAtCursor - moves a player in play to the free-camera cursor (the debug pause menu calls it for both players when the free camera turns off)
; In:  a1 = player record (g_Player1 / g_Player2)
; Out: the player's object (record +$2C) x/y = g_DebugCursorX/Y
; Evidence: btst #0,$25(a1) (in play), movea.w $2C(a1),a1, move.w $F41A,$3E(a1) / $F41E,$42(a1); static only (debug menu never ran)
; ---------------------------------------------------------------------------
Debug_PlacePlayerAtCursor:
	btst	#$0,$25(a1)
	beq.s	loc_015714
	movea.w	$2C(a1),a1
	move.w	(g_DebugCursorX).w,$3E(a1)
	move.w	(g_DebugCursorY).w,$42(a1)
loc_015714:
	rts
; ---------------------------------------------------------------------------
; Debug_CentreCursor - puts the free-camera cursor at the screen centre when the debug pause menu turns the free camera on
; In:  none
; Out: g_DebugCursorX = g_CameraX + $A0, g_DebugCursorY = g_CameraY + $70
; Evidence: branch from Debug_PauseMenu when not.b g_FreeCam leaves it set; static only
; ---------------------------------------------------------------------------
Debug_CentreCursor:
	move.w	(g_CameraX).w,d0
	addi.w	#$A0,d0
	move.w	d0,(g_DebugCursorX).w
	move.w	(g_CameraY).w,d0
	addi.w	#$70,d0
	move.w	d0,(g_DebugCursorY).w
	rts
