; ---------------------------------------------------------------------------
; Op2_SetSprVar0 - a2 op: sets sprite-block variable 0 ($22(a5)), one of three words ($22/$24/$26) that player natives step as animation phases
; In:  a2 = script: word value
; Out: NEXT
; Evidence: move.w (a2)+,$22(a5); the never-run ops after it clear/or/and/eor/inc/dec/test it; $104D8 steps $22(a5); 91 corpus runs; $48DCA `3FFE 0020 3FE4`
; ---------------------------------------------------------------------------
Op2_SetSprVar0:
	move.w	(a2)+,$22(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
