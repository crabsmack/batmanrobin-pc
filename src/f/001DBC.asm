; ---------------------------------------------------------------------------
; Mot_JumpLaunchSpeed - returns the launch speed of a jump: -(gravity d1) * runs to the peak, the runs being the time word just read (-2(a2)) / the tick
; In:  d1 = gravity (from Mot_JumpGravity); a2 = just past the time word; a4 = speed block
; Out: d1 = launch velocity (negative = up for a positive height); d0 clobbered
; Evidence: d0 = T * 256 / $2E(a4), d1 = -(d1 >> 4) * d0; callers Op2_ArcX, Op2_JumpAnim, Op2_Jump, Op2_FallAnim; executed in 137 corpus runs
; ---------------------------------------------------------------------------
Mot_JumpLaunchSpeed:
	moveq	#$0,d0
	move.w	-$2(a2),d0
	asl.l	#$8,d0
	divs.w	$2E(a4),d0
	asr.l	#$4,d1
	muls.w	d0,d1
	neg.l	d1
	rts
; ---------------------------------------------------------------------------
; Op2_SetTerrainBoxATouching - a2 op: sets and enables terrain box A (bit 1 of (a3)): offset dx/dy $40/$42, wall and floor masks $44/$46, with the contact results $52/$54 preset to the masks (starts as touching) and $50 cleared
; In:  a2 = script: word dx, word dy, word wall mask, word floor mask; a3 = motion block
; Out: NEXT
; Evidence: the move op $4CDA collides box A through Coll_MoveScene and clears the x/y velocity on wall/floor contact ($52/$54); 38 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetTerrainBoxATouching:
	clr.w	$50(a3)
	move.w	(a2)+,$40(a3)
	move.w	(a2)+,$42(a3)
	move.w	(a2)+,d0
	move.w	d0,$44(a3)
	move.w	d0,$52(a3)
	move.w	(a2)+,d0
	move.w	d0,$46(a3)
	move.w	d0,$54(a3)
	bset	#$1,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTerrainBoxA - a2 op: sets and enables terrain box A (bit 1 of (a3)): offset $40/$42, masks $44/$46, contact results $50/$52/$54 cleared
; In:  a2 = script: word dx, word dy, word wall mask, word floor mask; a3 = motion block
; Out: NEXT
; Evidence: as Op2_SetTerrainBoxATouching with cleared results; 65 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetTerrainBoxA:
	clr.w	$52(a3)
	clr.w	$54(a3)
	clr.w	$50(a3)
	move.w	(a2)+,$40(a3)
	move.w	(a2)+,$42(a3)
	move.w	(a2)+,$44(a3)
	move.w	(a2)+,$46(a3)
; ---------------------------------------------------------------------------
; Op2_EnableTerrainBoxA - a2 op: turns terrain box A on (bset #1,(a3)) with its offset, masks and contact results as they are (Op2_SetTerrainBoxA past its setup)
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: a computed-jump target in the corpus: NEXT dispatches from $188E, $20BE, $3E6C, $BE4, $F42, $1D02, 940 call-site log entries
; ---------------------------------------------------------------------------
Op2_EnableTerrainBoxA:
	bset	#$1,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_DisableTerrainBoxA - a2 op: turns terrain box A off (bclr #1,(a3)), so the move op no longer collides it
; In:  a2 = script (no operand); a3 = motion block
; Out: NEXT
; Evidence: $4CDA tests bit 1 of (a3); 35 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_DisableTerrainBoxA:
	bclr	#$1,(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetTerrainBoxAMasks - a2 op: changes terrain box A's wall and floor masks ($44/$46(a3)) only
; In:  a2 = script: word wall mask, word floor mask; a3 = motion block
; Out: NEXT
; Evidence: two word moves; the masks go to Coll_MoveScene in d6/d7; 89 corpus runs incl. the TAS
; ---------------------------------------------------------------------------
Op2_SetTerrainBoxAMasks:
	move.w	(a2)+,$44(a3)
	move.w	(a2)+,$46(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
