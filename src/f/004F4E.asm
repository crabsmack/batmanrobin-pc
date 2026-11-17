; ---------------------------------------------------------------------------
; Op2_PlaceBodyBoxes_Off - tail of Op2_PlaceBodyBoxes while the body-box off count (a4) runs: decrements it and unlinks every linked body box (the object cannot be hit)
; In:  a1 = body box handles, d0 = count - 1
; Out: NEXT
; Evidence: subq.b #1,(a4) / Hitbox_Unlink / clr.w $2(a0) loop; Obj_SetBodyBoxes clears (a4); never ran in the corpus (writer of (a4) not found: inferred)
; ---------------------------------------------------------------------------
Op2_PlaceBodyBoxes_Off:
	subq.b	#$1,(a4)
loc_004F50:
	movea.w	(a1)+,a0
	tst.w	$2(a0)
	beq.s	loc_004F60
	bsr.w	Hitbox_Unlink
	clr.w	$2(a0)
loc_004F60:
	dbra	d0,loc_004F50
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlaceBodyBox - a2 op: places and links one body box, chosen by index, at the object's position (as one step of Op2_PlaceBodyBoxes); while the off count (a4) runs it unlinks that box instead
; In:  a2 = script: word index (out of range: nothing)
; Out: NEXT
; Evidence: movea.w $4(a4,d0.w),a0 / moveq #0,d0 / beq Op2_PlaceBodyBoxes_Place; 2 corpus runs; $18212 `4F68 0000`
; ---------------------------------------------------------------------------
Op2_PlaceBodyBox:
	move.w	(a2)+,d0
	cmp.w	$2(a4),d0
	bge.w	VM2_Next
	add.w	d0,d0
	movea.w	$4(a4,d0.w),a0
	moveq	#$0,d0
	move.w	$3E(a6),d1
	move.w	$42(a6),d2
	tst.b	(a4)
	beq.w	Op2_PlaceBodyBoxes_Place
	subq.b	#$1,(a4)
	tst.w	$2(a0)
	beq.w	VM2_Next
	bsr.w	Hitbox_Unlink
	clr.w	$2(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_PlaceBodyBoxAtDrawn - a2 op: as Op2_PlaceBodyBox at the object's drawn position $4E/$50(a6)
; In:  a2 = script: word index
; Out: NEXT
; Evidence: move.w $4E(a6),d1 / move.w $50(a6),d2; 2 corpus runs; $18220 `4F9E 0001`
; ---------------------------------------------------------------------------
Op2_PlaceBodyBoxAtDrawn:
	move.w	(a2)+,d0
	cmp.w	$2(a4),d0
	bge.w	VM2_Next
	add.w	d0,d0
	movea.w	$4(a4,d0.w),a0
	moveq	#$0,d0
	move.w	$4E(a6),d1
	move.w	$50(a6),d2
	tst.b	(a4)
	beq.w	Op2_PlaceBodyBoxes_Place
	subq.b	#$1,(a4)
	tst.w	$2(a0)
	beq.w	VM2_Next
	bsr.w	Hitbox_Unlink
	clr.w	$2(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetOscX - a2 op: starts the x axis of the oscillator block in an a6 field: limit +4 = amplitude << 8, max speed +$C = word * speed $2E(a4), acceleration +$10 from Mot_AccelForDistance, position and velocity cleared
; In:  a2 = script: word field offset, word amplitude (8.8 px), word max speed, words distance and time (Mot_AccelForDistance)
; Out: NEXT (an empty field skips the 4 words)
; Evidence: asl.l #8 -> $4(a0) / mulu.w $2E(a4) -> $C(a0) / bsr Mot_AccelForDistance -> $10(a0) / clr.l (a0), $8(a0); stepped by Op2_StepOsc; 54 corpus runs; $1EECC `4FD4 000E 0300 0080 0080 0078`
; ---------------------------------------------------------------------------
Op2_SetOscX:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d0
	beq.w	loc_005006
	movea.w	d0,a0
	move.w	(a2)+,d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$4(a0)
	move.w	(a2)+,d0
	mulu.w	$2E(a4),d0
	move.l	d0,$C(a0)
	bsr.w	Mot_AccelForDistance
	move.l	d1,$10(a0)
	clr.l	(a0)
	clr.l	$8(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_005006:
	addq.l	#$8,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetOscY - a2 op: as Op2_SetOscX for the y axis of the oscillator block (+$18 limit, +$20 max speed, +$24 acceleration, +$14/+$1C cleared)
; In:  a2 = script: word field offset, word amplitude, word max speed, words distance and time
; Out: NEXT
; Evidence: same code on $14-$24(a0); 67 corpus runs; $133B2 `500C 000E 0200 00AA 00AA 0060`
; ---------------------------------------------------------------------------
Op2_SetOscY:
	move.w	(a2)+,d0
	move.w	(a6,d0.w),d0
	beq.w	loc_005040
	movea.w	d0,a0
	move.w	(a2)+,d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$18(a0)
	move.w	(a2)+,d0
	mulu.w	$2E(a4),d0
	move.l	d0,$20(a0)
	bsr.w	Mot_AccelForDistance
	move.l	d1,$24(a0)
	clr.l	$14(a0)
	clr.l	$1C(a0)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_005040:
	addq.l	#$8,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_AllocOsc - a2 op: makes sure an a6 field holds an oscillator block (allocates a pool block when it is 0; on an empty pool it just continues) and clears its 40 bytes
; In:  a2 = script: word field offset
; Out: NEXT
; Evidence: bsr ObjPool_Alloc / move.w a0,(a6,d1.w) / 10 clr longs; 70 corpus runs; $133AE `5046 000E 500C 000E`
; ---------------------------------------------------------------------------
Op2_AllocOsc:
	move.w	(a2)+,d1
	move.w	(a6,d1.w),d0
	beq.s	loc_00506A
	movea.w	d0,a0
loc_005050:
	moveq	#$0,d0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00506A:
	bsr.w	ObjPool_Alloc
	bne.w	VM2_Next
	move.w	a0,(a6,d1.w)
	bra.s	loc_005050
