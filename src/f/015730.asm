; ---------------------------------------------------------------------------
; Debug_FreeCamStep - free-camera frame (instead of the objects while g_FreeCam): the D-pad accelerates a cursor (with terrain collision while A is held), C toggles a camera lock, cursor sprites and hex readouts are drawn and the camera follows the cursor
; In:  g_DebugPadPtr = pad record
; Out: g_DebugCursorX/Y moved; camera scrolled right up to 4 px; Camera_UpdateView
; Evidence: speed $F422 += $2000 up to 4.0 while a direction is held; A held: Coll_MoveScene with masks $FFFF; frames $AA (cursor) and $B4 (16-px block) via Spr_AddSorted; Debug_DrawHexBytes of g_CameraLockX, cursor offset/position/block and $E00A/$E00E + cursor; callers Frame_RunFreeCam, $32F14, $33274 when g_FreeCam; static only
; ---------------------------------------------------------------------------
Debug_FreeCamStep:
	movea.w	(g_DebugPadPtr).w,a0
	btst	#$5,$2(a0)
	beq.s	loc_015758
	tst.l	(g_CameraLockX).w
	beq.s	loc_015748
	clr.l	(g_CameraLockX).w
	bra.s	loc_015758
loc_015748:
	moveq	#$0,d0
	move.w	(g_CameraX).w,d0
	addi.w	#$A0,d0
	swap	d0
	move.l	d0,(g_CameraLockX).w
loc_015758:
	move.b	(a0),d0
	andi.w	#$F,d0
	bne.s	loc_015768
	clr.l	(g_DebugCursorSpeed).w
	bra.w	loc_0157E4
loc_015768:
	moveq	#$0,d1
	moveq	#$0,d2
	move.l	(g_DebugCursorSpeed).w,d3
	cmpi.l	#$40000,d3
	bge.s	loc_015780
	addi.l	#$2000,(g_DebugCursorSpeed).w
loc_015780:
	btst	#$2,d0
	beq.s	loc_015788
	sub.l	d3,d1
loc_015788:
	btst	#$3,d0
	beq.s	loc_015790
	add.l	d3,d1
loc_015790:
	btst	#$0,d0
	beq.s	loc_015798
	sub.l	d3,d2
loc_015798:
	btst	#$1,d0
	beq.s	loc_0157A0
	add.l	d3,d2
loc_0157A0:
	btst	#$6,(a0)
	bne.s	loc_0157B0
	add.l	d1,(g_DebugCursorX).w
	add.l	d2,(g_DebugCursorY).w
	bra.s	loc_0157E4
loc_0157B0:
	move.l	(g_DebugCursorX).w,d4
	move.l	(g_DebugCursorY).w,d5
	add.l	d4,d1
	add.l	d5,d2
	move.w	d1,(g_DebugCursorX+2).w
	move.w	d2,(g_DebugCursorY+2).w
	swap	d1
	swap	d2
	swap	d4
	swap	d5
	sub.w	d4,d1
	sub.w	d5,d2
	move.w	#$FFFF,d6
	move.w	#$FFFF,d7
	bsr.w	Coll_MoveScene
	move.w	d4,(g_DebugCursorX).w
	move.w	d5,(g_DebugCursorY).w
loc_0157E4:
	move.w	(g_DebugCursorX).w,d4
	move.w	(g_DebugCursorY).w,d5
	move.w	#$8000,d6
	move.w	#$7FFF,d7
	move.w	#$AA,d1
	move.w	(g_DebugCursorFrame).w,d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(g_DebugCursorFrame).w
	jsr	(Spr_AddSorted).l
	andi.w	#$FFF0,d4
	andi.w	#$FFF0,d5
	move.w	d4,(g_DebugCursorBlockX).w
	move.w	d5,(g_DebugCursorBlockY).w
	move.w	#$B4,d1
	move.w	(g_DebugCursorFrame).w,d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(g_DebugCursorFrame).w
	jsr	(Spr_AddSorted).l
	lea.l	(g_CameraLockX).w,a6
	move.w	#$2317,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugReadout).w,a6
	move.w	(g_DebugCursorX).w,d0
	sub.w	(g_CameraX).w,d0
	subi.w	#$A0,d0
	move.w	d0,(a6)
	move.w	#$1E18,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugReadout).w,a6
	move.w	(g_DebugCursorY).w,d0
	sub.w	(g_CameraY).w,d0
	subi.w	#$70,d0
	move.w	d0,(a6)
	move.w	#$2318,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugCursorX).w,a6
	move.w	#$1E1A,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugCursorY).w,a6
	move.w	#$231A,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugCursorBlockX).w,a6
	move.w	#$1E19,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugCursorBlockY).w,a6
	move.w	#$2319,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugCursorMapX).w,a6
	move.l	(g_MapScrollX).w,d0
	add.l	(g_DebugCursorX).w,d0
	move.l	d0,(a6)
	move.w	#$1E15,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	lea.l	(g_DebugCursorMapY).w,a6
	move.l	(g_MapScrollY).w,d0
	add.l	(g_DebugCursorY).w,d0
	move.l	d0,(a6)
	move.w	#$2315,d0
	move.w	#$2,d1
	bsr.w	Debug_DrawHexBytes
	move.w	(g_CameraLockX).w,d0
	bne.s	loc_0158FE
	move.w	(g_DebugCursorX).w,d0
loc_0158FE:
	subi.w	#$A0,d0
	sub.w	(g_CameraX).w,d0
	bpl.s	Debug_FreeCamScroll
	bra.s	Debug_FreeCamSyncLayer
