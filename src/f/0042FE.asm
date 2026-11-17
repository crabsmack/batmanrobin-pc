; ---------------------------------------------------------------------------
; Op2_BranchNotSection - a2 op: branches unless the current game section g_SectionId equals the operand
; In:  a2 = script: word section id, word offset
; Out: branch when different, else NEXT after the operands
; Evidence: cmp.w g_SectionId,d0 / bne Op2_Goto; 15 corpus runs; $1B8D4 `42FE 1100 000C` (stage 1 section $1100)
; ---------------------------------------------------------------------------
Op2_BranchNotSection:
	move.w	(a2)+,d0
	cmp.w	(g_SectionId).w,d0
	bne.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetShakeTimer - a2 op: sets the sprite's shake timer (byte $17(a5)): while it runs Op2_Draw draws the sprite displaced by a random -2..1 px
; In:  a2 = script: word frames (low byte used)
; Out: NEXT
; Evidence: move.b d0,$17(a5); Op2_Draw decrements $17(a5) and adds Rand_Next>>14 to x/y; 74 corpus runs; $19844 `430E 001E`
; ---------------------------------------------------------------------------
Op2_SetShakeTimer:
	move.w	(a2)+,d0
	move.b	d0,$17(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
