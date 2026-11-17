; ---------------------------------------------------------------------------
; Camera_TrackPlayersX - per frame: scrolls the camera right toward the players (their mean x when both play) or the lock point, then Camera_UpdateView
; In:  g_Player1/2 records (+$25 bit 0 in play, +$2E x)
; Out: g_CameraX, g_CameraSpeedX
; Evidence: target - $A0 - camera: <= 0 stops (never scrolls left); <= $C000 snaps; else speed = 3/32 of it eased by $3000 per frame up to 4 px; g_CameraLockX overrides the players; called by the main gameplay loop $9BC0 every frame (121825 TAS frames)
; ---------------------------------------------------------------------------
Camera_TrackPlayersX:
	btst	#$0,(g_P1Flags).w
	beq.s	loc_00945E
	move.l	($FFFFF67E).w,d0
	btst	#$0,(g_P2Flags).w
	beq.s	loc_00946A
	move.l	($FFFFF6BE).w,d1
	lsr.l	#$1,d0
	lsr.l	#$1,d1
	add.l	d1,d0
	bra.s	loc_00946A
loc_00945E:
	move.l	($FFFFF6BE).w,d0
	btst	#$0,(g_P2Flags).w
	beq.s	loc_00947E
loc_00946A:
	move.l	(g_CameraLockX).w,d1
	beq.s	loc_009472
	move.l	d1,d0
loc_009472:
	subi.l	#$A00000,d0
	sub.l	(g_CameraX).w,d0
	bgt.s	loc_009484
loc_00947E:
	clr.l	(g_CameraSpeedX).w
	bra.s	loc_0094D4
loc_009484:
	cmpi.l	#$C000,d0
	ble.s	loc_0094CA
	asr.l	#$3,d0
	move.l	d0,d1
	asr.l	#$2,d1
	sub.l	d1,d0
	move.l	(g_CameraSpeedX).w,d1
	cmp.l	d0,d1
	blt.s	loc_0094AA
	subi.l	#$3000,d1
	cmp.l	d0,d1
	bgt.s	loc_0094C4
	move.l	d0,d1
	bra.s	loc_0094C4
loc_0094AA:
	addi.l	#$3000,d1
	cmp.l	d0,d1
	blt.s	loc_0094B6
	move.l	d0,d1
loc_0094B6:
	cmpi.l	#$40000,d1
	blt.s	loc_0094C4
	move.l	#$40000,d1
loc_0094C4:
	move.l	d1,(g_CameraSpeedX).w
	move.l	d1,d0
loc_0094CA:
	add.l	d0,(g_CameraX).w
	bpl.s	loc_0094D4
	clr.l	(g_CameraX).w
loc_0094D4:
	bra.w	Camera_UpdateView
; ---------------------------------------------------------------------------
; Camera_TrackPlayersY - scrolls the camera vertically toward the players' y (mean when both play), speed limited near the top 0 and bottom $A8
; In:  g_Player1/2 records (+$32 y)
; Out: g_CameraY (clamped 0..$A8), g_CameraSpeedY
; Evidence: aims the players $70 below the top; max speed (distance to the limit)/16 + 0.5, at most 4 px; only caller the stage loop $33274
; ---------------------------------------------------------------------------
Camera_TrackPlayersY:
	btst	#$0,(g_P1Flags).w
	beq.s	loc_0094F8
	move.l	($FFFFF682).w,d0
	btst	#$0,(g_P2Flags).w
	beq.s	loc_009502
	move.l	($FFFFF6C2).w,d1
	lsr.l	#$1,d0
	lsr.l	#$1,d1
	add.l	d1,d0
	bra.s	loc_009502
loc_0094F8:
	move.l	($FFFFF6C2).w,d0
	btst	#$0,(g_P2Flags).w
loc_009502:
	subi.l	#$700000,d0
	move.l	(g_CameraSpeedY).w,d2
	move.l	(g_CameraY).w,d3
	move.l	#$4000,d4
	move.l	#$A80000,d5
	move.l	d3,d6
	move.l	d5,d7
	sub.l	d3,d7
	asr.l	#$4,d6
	asr.l	#$4,d7
	addi.l	#$8000,d6
	addi.l	#$8000,d7
	cmpi.l	#$40000,d6
	ble.s	loc_009540
	move.l	#$40000,d6
loc_009540:
	cmpi.l	#$40000,d7
	ble.s	loc_00954E
	move.l	#$40000,d7
loc_00954E:
	sub.l	d3,d0
	move.l	d0,d2
	bpl.s	loc_00955E
	neg.l	d6
	cmp.l	d6,d2
	bge.s	loc_009564
	move.l	d6,d2
	bra.s	loc_009564
loc_00955E:
	cmp.l	d7,d2
	ble.s	loc_009564
	move.l	d7,d2
loc_009564:
	move.l	d2,(g_CameraSpeedY).w
	add.l	d2,d3
	bpl.s	loc_009570
	moveq	#$0,d3
	bra.s	loc_009576
loc_009570:
	cmp.l	d5,d3
	ble.s	loc_009576
	move.l	d5,d3
loc_009576:
	move.l	d3,(g_CameraY).w
	rts
