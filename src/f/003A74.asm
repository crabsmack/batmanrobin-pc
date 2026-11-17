; ---------------------------------------------------------------------------
; Op2_SwitchAttacker - a2 op: branches by who struck the object: compares the attacker id of g_StruckBox (+$1A) with a list of (id, offset) pairs and takes the first match
; In:  a2 = script: word count-1, then pairs word id, word offset
; Out: branch on a match, else NEXT after the list
; Evidence: move.w $1A(a0),d0 / cmp.w (a2)+ / beq Op2_Goto / dbra; 33 runs; $1AFB6 `3A74 0001 0001 0036 0002 0032` (player 1 / player 2)
; ---------------------------------------------------------------------------
Op2_SwitchAttacker:
	movea.l	(g_StruckBox).w,a0
	move.w	$1A(a0),d0
	move.w	(a2)+,d7
loc_003A7E:
	cmp.w	(a2)+,d0
	beq.w	Op2_Goto
	addq.l	#$2,a2
	dbra	d7,loc_003A7E
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetVector0 - a2 op: sets the object's script vector 0 ($32(a6)) to a relative script address, the target of Op2_TickTimer0GotoV0 and $3B06/$3B1E
; In:  a2 = script: word offset
; Out: NEXT after the operand
; Evidence: movea.l a2,a0 / adda.w (a2)+,a0 / move.l a0,$32(a6); 42 runs
; ---------------------------------------------------------------------------
Op2_SetVector0:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$32(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetVector1 - a2 op: sets the object's script vector 1 ($36(a6)) to a relative script address, the target of Op2_TickTimer0GotoV1 and $3B0E/$3B28
; In:  a2 = script: word offset
; Out: NEXT after the operand
; Evidence: move.l a0,$36(a6); 53 runs
; ---------------------------------------------------------------------------
Op2_SetVector1:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$36(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
