; ---------------------------------------------------------------------------
; ToyFigure_DrawSpin1 - anim-script native: turns a toy figure one step further round (sprite var 0 $22(a5) + 1, mod 60) and draws that rotation frame
; In:  a5 = sprite block, a4/a6 = object, a1 = the next frame slot (from Op2_AnimFrame)
; Out: $22(a5) = new index; sprite added (falls into ToyFigure_DrawIndex)
; Evidence: addq #1 then subtract $3C until < 60; called by Op2_CallNativeRel in the figure animation script $036CBC (body $036BCE); TAS f91205-f93246 (191 frames), stage 3
; ---------------------------------------------------------------------------
ToyFigure_DrawSpin1:
	move.w	$22(a5),d1
	addq.w	#$1,d1
	andi.w	#$7F,d1
loc_036CDE:
	cmpi.w	#$3C,d1
	bcs.s	ToyFigure_DrawIndex
	subi.w	#$3C,d1
	bra.s	loc_036CDE
; ---------------------------------------------------------------------------
; ToyFigure_DrawSpin3 - anim-script native: turns a toy figure three steps (18 degrees) further round, mod 60, and draws that rotation frame
; In:  a5 = sprite block, a1 = the next frame slot
; Out: $22(a5) = new index; sprite added via ToyFigure_DrawIndex
; Evidence: addq #3 then mod $3C; called from the figure animation script $036CC8 and from Scr2_035D74+$76 (relative word); never a separate function-start in func_frames (interior entry, ctarget)
; ---------------------------------------------------------------------------
ToyFigure_DrawSpin3:
	move.w	$22(a5),d1
	addq.w	#$3,d1
	andi.w	#$7F,d1
loc_036CF4:
	cmpi.w	#$3C,d1
	bcs.s	ToyFigure_DrawIndex
	subi.w	#$3C,d1
	bra.s	loc_036CF4
; ---------------------------------------------------------------------------
; ToyFigure_SetTumbleDivisor - anim-script native: sets the velocity-to-frame divisor of a tumbling toy figure: $36(a5) = bias $32(a5) / 30, so a y speed in [-bias, +bias] maps onto the 60 rotation frames
; In:  a5 = sprite block (+$32.l bias)
; Out: $36(a5) = divisor
; Evidence: divs.w #$1E; the figure animation script $036CAC calls it before ToyFigure_DrawByVelY, which divides by $36(a5); NOTES a5 +$32/+$36
; ---------------------------------------------------------------------------
ToyFigure_SetTumbleDivisor:
	move.l	$32(a5),d1
	divs.w	#$1E,d1
	move.w	d1,$36(a5)
	rts
; ---------------------------------------------------------------------------
; ToyFigure_DrawByVelY - anim-script native: picks a toy figure's rotation frame from its vertical motion: (y speed $1C(a3) + y accel $28(a3) + bias $32(a5)) / divisor $36(a5), 0 when out of 0-59, and draws it (tumbling through a jump)
; In:  a3 = motion block, a5 = sprite block, a1 = the next frame slot
; Out: $22(a5) = index; sprite added via ToyFigure_DrawIndex
; Evidence: divu.w $36(a5), cmpi #$3C; called from the figure animation script $036CB4 (after ToyFigure_SetTumbleDivisor) while the figure jumps (Op2_JumpAnim in its hop coroutines)
; ---------------------------------------------------------------------------
ToyFigure_DrawByVelY:
	move.l	$1C(a3),d1
	add.l	$28(a3),d1
	add.l	$32(a5),d1
	bmi.s	loc_036D26
	divu.w	$36(a5),d1
	cmpi.w	#$3C,d1
	bcs.s	ToyFigure_DrawIndex
loc_036D26:
	move.w	#$0,d1
	bra.s	ToyFigure_DrawIndex
; ---------------------------------------------------------------------------
; ToyFigure_DrawAngle - anim-script native: draws a toy figure at the rotation index given by the next script word (0-59, 6 degrees each)
; In:  a2 = script (word index), a5 = sprite block, a1 = the next frame slot
; Out: a2 past the word; $22(a5) = index; sprite added via ToyFigure_DrawIndex
; Evidence: move.w (a2)+,d1; the figure animation script $036C9E passes 0 (upright)
; ---------------------------------------------------------------------------
ToyFigure_DrawAngle:
	move.w	(a2)+,d1
; ---------------------------------------------------------------------------
; ToyFigure_DrawIndex - shared tail of the toy-figure draw natives: looks up rotation index d1 in Tbl_036DA8 (frame word, attr AND mask, flip XOR byte; 16 frames + flips make 60 steps) or, when the figure is charged, in the outlined copy at Tbl_ToyFigureChargedFrames, swaps the frame and adds the sprite
; In:  d1 = rotation index 0-59; a4 +$22 script var 0: bit 2 = charged (outlined frames), bit 3 = warning blink (outlined on random frames while frame counter bit 3 is set); a5 = sprite block; a1 = frame slot
; Out: $22(a5) = d1; frame slot updated; sprite added at the object's x/y $3E/$42 (Spr_AddBack when depth $3A(a5) = 0, else Spr_AddSorted); a1 advanced
; Evidence: andi #$C / #$4 / #$8 tests on $22(a4), g_FrameCounter low word bit 3 and Rand_Next bit 0; frames $4344-$43DA (plain) and $43E4-$447A (outlined, white/blue edge) rendered from Tbl_1DDB60: a toy figure with arms out, striped shirt, blue trousers
; ---------------------------------------------------------------------------
ToyFigure_DrawIndex:
	move.w	d1,$22(a5)
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	lea.l	(Tbl_036DA8).l,a0
	move.w	$22(a4),d0
	andi.w	#$C,d0
	beq.s	loc_036D78
	andi.w	#$4,d0
	bne.s	loc_036D68
	move.w	(g_FrameCounter+2).w,d0
	andi.w	#$8,d0
	beq.s	loc_036D78
	jsr	(Rand_Next).l
	andi.w	#$1,d0
	beq.s	loc_036D78
	bra.s	loc_036D72
loc_036D68:
	move.w	(g_FrameCounter+2).w,d0
	andi.w	#$4,d0
	beq.s	loc_036D78
loc_036D72:
	lea.l	(Tbl_ToyFigureChargedFrames).l,a0
loc_036D78:
	add.w	d1,d1
	add.w	d1,d1
	adda.w	d1,a0
	move.w	(a0)+,d1
	move.b	$38(a5),d6
	and.b	(a0)+,d6
	move.b	(a0)+,d0
	eor.b	d0,d6
	lsl.w	#$8,d6
	move.w	(a1),d2
	jsr	(Spr_ChangeFrame).l
	move.w	d1,(a1)+
	move.w	$3A(a5),d7
	bne.s	loc_036DA2
	jmp	(Spr_AddBack).l
loc_036DA2:
	jmp	(Spr_AddSorted).l
