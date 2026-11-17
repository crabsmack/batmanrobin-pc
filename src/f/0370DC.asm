; ---------------------------------------------------------------------------
; ToyFigure_FxSpinOrKill - a4-script native for a toy figure flung off as an effect: deletes it (pops the script return and jumps to Op4_Kill) once it leaves the screen (x - g_OnScreenLeft >= $170 or y >= $E6), otherwise spins it 5 steps (30 degrees) a frame through Tbl_036DA8
; In:  a6 = a4 effect object (+$1C byte rotation index), d4/d5 = its drawn x/y (from the motion op before it)
; Out: $1C(a6) index, $1E(a6) wanted frame, $22(a6) attr flip bits (keeps its palette bits $60)
; Evidence: addq.b #5 mod $3C, Tbl_036DA8 lookup; called by Op4_CallNative (dc.l $0370DC) in Tbl_ToyFigureChargedFrames at $037012 before Op4_DrawFrame; TAS f79698-f112962 (6940 frames)
; ---------------------------------------------------------------------------
ToyFigure_FxSpinOrKill:
	move.w	d4,d0
	sub.w	(g_OnScreenLeft).w,d0
	cmpi.w	#$170,d0
	bcc.s	loc_0370EE
	cmpi.w	#$E6,d5
	blt.s	loc_0370F6
loc_0370EE:
	addq.l	#$4,a7
	jmp	(Op4_Kill).l
loc_0370F6:
	moveq	#$0,d0
	move.b	$1C(a6),d0
	addq.b	#$5,d0
	cmpi.b	#$3C,d0
	bcs.s	loc_03710E
loc_037104:
	subi.b	#$3C,d0
	cmpi.b	#$3C,d0
	bcc.s	loc_037104
loc_03710E:
	move.b	d0,$1C(a6)
	lea.l	Tbl_036DA8(pc),a0
	add.w	d0,d0
	add.w	d0,d0
	adda.w	d0,a0
	move.w	(a0)+,$1E(a6)
	move.b	$22(a6),d0
	andi.b	#$60,d0
	and.b	(a0)+,d0
	move.b	(a0)+,d1
	eor.b	d1,d0
	move.b	d0,$22(a6)
	rts
; ---------------------------------------------------------------------------
; ToyChain_SpawnInline - inline-argument spawn of a toy-figure chain: called by jsr with dc.w link count, dc.l parameter table, dc.l a2 script after it; makes an unlinked 5-block boss-class object (blocks a6/a3/a4/a5 + chain block $E(a6)) and fills the chain block from the table, then returns to the caller's caller
; In:  return address = the inline arguments; table words: var 0 flags, base heading, heading word (<<16 | $8000), sway scale +$8, +$C, grow speeds (<<8) +$10/+$14, energy per link (also $52(a6)), link damage/knockback args +$22/+$24, long link hit callback +$26, blink reload +$1E, blink pattern +$18
; Out: d0 = 0 and a0 = the chain block (Z set), or nonzero when the pool is full; $2A(a4) = script; $58(a6) = $B; $22(a5) = $AAAA (touch-test pattern); $2A/$2C(block) = links to grow / maximum
; Evidence: movea.l (a7)+,a2 / Obj_SpawnUnlinked d7 = 5 / ObjPool_Clear; 46 jsr sites $0398BA-$03B0EE (stage 3 chain spawners), e.g. $0398BA: dc.w 5, dc.l $039AC4, dc.l $0398C8; used as Op2_SpawnChild spawners by the chain bases (Scr2_0397A6 ...)
; ---------------------------------------------------------------------------
ToyChain_SpawnInline:
	movea.l	(a7)+,a2
	moveq	#$0,d4
	moveq	#$0,d5
	lea.l	(VM2_RunObject).l,a0
	moveq	#$5,d7
	jsr	(Obj_SpawnUnlinked).l
	bne.w	loc_0371C4
	movea.w	$E(a6),a0
	jsr	(ObjPool_Clear).l
	move.w	#$B,$58(a6)
	move.w	(a2)+,d0
	movea.l	(a2)+,a1
	move.l	(a2)+,$2A(a4)
	move.w	#$AAAA,$22(a5)
	move.w	d0,$2A(a0)
	move.w	d0,$2C(a0)
	clr.w	$24(a4)
	move.w	(a1)+,$22(a4)
	move.w	(a1)+,(a0)
	move.w	(a1)+,d0
	swap	d0
	move.w	#$8000,d0
	move.l	d0,$4(a0)
	move.w	(a1)+,$8(a0)
	move.w	(a1)+,$C(a0)
	move.w	(a1)+,d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$10(a0)
	move.w	(a1)+,d0
	ext.l	d0
	asl.l	#$8,d0
	move.l	d0,$14(a0)
	move.w	(a1)+,d0
	move.w	d0,$20(a0)
	move.w	d0,$52(a6)
	move.w	(a1)+,$22(a0)
	move.w	(a1)+,$24(a0)
	move.l	(a1)+,$26(a0)
	move.w	(a1)+,$1E(a0)
	move.w	(a1)+,$18(a0)
	moveq	#$0,d0
loc_0371C4:
	rts
; ---------------------------------------------------------------------------
; ToyChain_KillChild - Op2_CallNativeOnChild callback that destroys a chain child: clears the owner's field, removes every link (frames, boxes, blocks) and destroys the child object
; In:  a6 = owner, d0 = the owner's child field offset, a0 = the chain object
; Out: the chain object destroyed (Obj_DestroyLoaded -> Obj_Destroy, restores a1-a6)
; Evidence: clr.w (a6,d0.w); bsr ToyChain_RemoveAllLinks; jmp Obj_DestroyLoaded; dc.l $0371C6 after Op2_CallNativeOnChild $002A in Scr2_037B12+$4C and Scr2_037BC4+$A4/$2F0/$2F8 (owners that carry a chain)
; ---------------------------------------------------------------------------
ToyChain_KillChild:
	clr.w	(a6,d0.w)
	movem.l	a1-a6,-(a7)
	movea.l	a0,a6
	movea.w	$8(a6),a3
	movea.w	$A(a6),a4
	movea.w	$C(a6),a5
	bsr.w	ToyChain_RemoveAllLinks
	jmp	(Obj_DestroyLoaded).l
