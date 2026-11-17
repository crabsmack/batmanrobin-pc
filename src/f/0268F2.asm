; ---------------------------------------------------------------------------
; Enemy_LadderThug_Throw - ladder-thug native: runs the a4 throw script kept in its coroutine slot 3 ($44(a4)) at its drawn point + two inline words: a random one of 3-6 bombs arcing (Arc_PlanToArgs) to fixed floor points
; In:  a2 = inline dx, dy words, a6 +$4E/$50 drawn point
; Out: a2 past the words; Fx_RunScript
; Evidence: op $2CB2 at $026800 (dx -$C, dy -5) and $0268E6 in the throw animations; slot 3 set by Op2_StartCoroutine3Rel in each spawner script ($0278EA, $0278FA, $027C28); thrown objects $026AEC step Arc_Step and Op4_TerrainMove; TAS f25249-f33867 (7)
; ---------------------------------------------------------------------------
Enemy_LadderThug_Throw:
	moveq	#$0,d2
	moveq	#$0,d3
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	add.w	(a2)+,d4
	add.w	(a2)+,d5
	swap	d4
	swap	d5
	movea.l	$44(a4),a0
	jmp	(Fx_RunScript).l
; ---------------------------------------------------------------------------
; Enemy_LadderThug_ReleaseLadder - ladder-thug native before Op2_Kill: releases the art of the 10 ladder-segment sprite frames kept in its ladder block (+$8..)
; In:  a6 +$10 = ladder block
; Out: -
; Evidence: op $2CB2 at $02669A, then Op2_Kill; Spr_ReleaseFrame x10 (dbra 9)
; ---------------------------------------------------------------------------
Enemy_LadderThug_ReleaseLadder:
	movea.w	$10(a6),a1
	move.w	#$9,d7
	lea.l	$8(a1),a1
loc_026920:
	move.w	(a1)+,d2
	jsr	(Spr_ReleaseFrame).l
	dbra	d7,loc_026920
	rts
; ---------------------------------------------------------------------------
; Enemy_LadderThug_SwingLadder - ladder-thug native every frame: swings the rope ladder as a pendulum (angle +0 / speed +4 of the ladder block, pushed back toward 0 by $180) and draws its 10 segments along the curving chain (frame by segment angle from the table at $026A44); segment 5 sets the drawn point $4E/$50 where the thug hangs
; In:  a6 = object (+$10 ladder block, +$3E/$42 top x/y)
; Out: segment frames in the ladder block; $4E/$50(a6); a2-a6 preserved
; Evidence: op $2CB2 at $02663A and $02668E in both ladder loops; Tbl_059B22 sine/cos; Spr_ChangeFrame + Spr_AddSorted per segment; png f025250 (rope ladder)
; ---------------------------------------------------------------------------
Enemy_LadderThug_SwingLadder:
	movem.l	a2-a6,-(a7)
	movea.w	$10(a6),a1
	move.l	#$180,d0
	move.l	$4(a1),d1
	move.l	(a1),d2
	bmi.s	loc_026946
	neg.l	d0
loc_026946:
	add.l	d0,d1
	move.l	d1,$4(a1)
	add.l	d1,d2
	move.l	d2,(a1)
	move.l	d2,d3
	add.l	d2,d2
	add.l	d2,d2
	addi.l	#$1008000,d2
	move.l	d2,d0
	swap	d0
	andi.w	#$3FE,d0
	lea.l	(Tbl_059B22).l,a0
	adda.w	d0,a0
	move.w	$100(a0),d4
	ext.l	d4
	asl.l	#$7,d4
	move.w	(a0),d5
	ext.l	d5
	asl.l	#$7,d5
	add.l	$3E(a6),d4
	add.l	$42(a6),d5
	move.w	#$9,d7
	lea.l	$8(a1),a1
loc_02698A:
	move.l	d2,d0
	swap	d0
	andi.w	#$3FE,d0
	move.w	d0,d1
	lea.l	(Tbl_059B22).l,a0
	adda.w	d0,a0
	move.w	$100(a0),d0
	ext.l	d0
	asl.l	#$5,d0
	add.l	d0,d4
	move.w	(a0),d0
	ext.l	d0
	asl.l	#$5,d0
	add.l	d0,d5
	subi.w	#$80,d1
	ext.l	d1
	divs.w	#$D,d1
	bpl.s	loc_0269BC
	moveq	#$0,d1
loc_0269BC:
	cmpi.w	#$13,d1
	ble.s	loc_0269C6
	move.w	#$13,d1
loc_0269C6:
	add.w	d1,d1
	add.w	d1,d1
	dc.w	$4CBB,$0042,$1078	; insn: movem.w $26a44(pc, d1.w), d1/d6
	cmpi.l	#$FFF00000,d5
	blt.s	loc_0269F8
	movem.l	d2-d5/d7,-(a7)
	move.w	(a1),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a1)+
	move.w	#$100,d7
	swap	d4
	swap	d5
	jsr	(Spr_AddSorted).l
	movem.l	(a7)+,d2-d5/d7
loc_0269F8:
	cmpi.w	#$4,d7
	bne.s	loc_026A3A
	move.l	d2,d0
	swap	d0
	subi.w	#$80,d0
	andi.w	#$3FE,d0
	lea.l	(Tbl_059B22).l,a0
	adda.w	d0,a0
	move.w	$100(a0),d0
	ext.l	d0
	asl.l	#$4,d0
	add.l	d4,d0
	swap	d0
	addi.w	#$FFF3,d0
	move.w	d0,$4E(a6)
	move.w	(a0),d0
	ext.l	d0
	asl.l	#$4,d0
	add.l	d5,d0
	swap	d0
	addi.w	#$1C,d0
	move.w	d0,$50(a6)
	add.l	d3,d3
loc_026A3A:
	add.l	d3,d2
	dbra	d7,loc_02698A
	movem.l	(a7)+,a2-a6
	rts
