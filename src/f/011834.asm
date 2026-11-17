; ---------------------------------------------------------------------------
; HUD_StepPanelSlide - moves one player's HUD panel a frame along its slide: sliding out (+$25 bit 6) it speeds up until 104 px past home, sliding in it brakes to a stop
; In:  a6 = player record (+$A home x, +$E.l x 16.16, +$12.l speed, +$16.l acceleration, +$25 bit 2 right-side panel, bit 6 sliding out)
; Out: +$E/+$12 updated; bit 6 and the speed cleared once the outgoing panel passes home x -/+ $68
; Evidence: called by HUD_Update for g_Player1 and g_Player2 every frame (TAS 532-130772); the speed it zeroes is what HudOp_WaitSlide polls; speeds are set by HUD_StartSlideOut/HUD_StartSlideIn
; ---------------------------------------------------------------------------
HUD_StepPanelSlide:
	btst	#$6,$25(a6)
	beq.s	loc_01187A
	move.l	$12(a6),d0
	add.l	$16(a6),d0
	move.l	$E(a6),d1
	add.l	d0,d1
	move.l	d1,d2
	swap	d2
	sub.w	$A(a6),d2
	btst	#$2,$25(a6)
	bne.s	loc_011862
	addi.w	#$68,d2
	bpl.s	loc_011870
	bra.s	loc_011868
loc_011862:
	subi.w	#$68,d2
	bmi.s	loc_011870
loc_011868:
	bclr	#$6,$25(a6)
	moveq	#$0,d0
loc_011870:
	move.l	d0,$12(a6)
	move.l	d1,$E(a6)
	rts
loc_01187A:
	move.l	$12(a6),d0
	beq.s	loc_01189A
	bpl.s	loc_01188A
	sub.l	$16(a6),d0
	bmi.s	loc_011892
	bra.s	loc_011890
loc_01188A:
	sub.l	$16(a6),d0
	bpl.s	loc_011892
loc_011890:
	moveq	#$0,d0
loc_011892:
	move.l	d0,$12(a6)
	add.l	d0,$E(a6)
loc_01189A:
	rts
