; ---------------------------------------------------------------------------
; Stage1_SignDebrisRun - run-list handler of the sign debris: counts its 70-frame life down, then releases the shared frame and unlinks; otherwise steps and draws the 18 pieces
; In:  a6 = debris object (+$18 frame slot, +$1A frames left, +$8.. 6 sprite-block handles)
; Out: pieces moved and drawn
; Evidence: installed by Stage1_SpawnSignDebris; TAS f1830-f4353 (744 frames), the burst under the blasted signs (png f002400)
; ---------------------------------------------------------------------------
Stage1_SignDebrisRun:
	subq.w	#$1,$1A(a6)
	bgt.s	loc_020A28
	move.w	$18(a6),d2
	jsr	(Spr_ReleaseFrame).l
	jmp	(Obj_UnlinkFreeWithBlocks).l
loc_020A28:
	lea.l	$8(a6),a4
	move.w	#$5,d7
loc_020A30:
	move.w	d7,-(a7)
	movea.w	(a4)+,a5
	bsr.w	Stage1_StepSignDebris
	bsr.w	Stage1_StepSignDebris
	bsr.w	Stage1_StepSignDebris
	move.w	(a7)+,d7
	dbra	d7,loc_020A30
	rts
