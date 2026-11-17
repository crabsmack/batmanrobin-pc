; ---------------------------------------------------------------------------
; Op2_StartA6Script - a2 op: starts a loader script on the global a6 VM (decompress/DMA jobs such as op $7120): stores its address in $E036, sets the per-frame resume hook $E03A = $70A8 (run from Frame_End by $7086) and the busy flag $F4BA
; In:  a2 = script: long a6 script address
; Out: NEXT
; Evidence: move.l (a2)+,$E036 / move.l #$70A8,$E03A / st $F4BA; $70A8 resumes a6 at $E036, $70B8 (the a6 end op) clears $F4BA; 7 corpus runs; $18078 `43E2 0001 FB1E 43F6` (a6 script $1FB1E: `7120 ...`)
; ---------------------------------------------------------------------------
Op2_StartA6Script:
	move.l	(a2)+,(g_A6ScriptPc).w
	move.l	#$70A8,(g_A6ResumeHook).w
	st.b	(g_A6ScriptBusy).w
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitA6Script - a2 op: waits (yielding, retried every frame) until the global a6 loader script has finished ($F4BA clear)
; In:  a2 = script (no operand)
; Out: NEXT when done; else a2 back on this op and Op2_Yield
; Evidence: tst.b $F4BA / beq $7E0 / subq.l #2,a2 / bra Op2_Yield; 2 corpus runs; follows Op2_StartA6Script at $1807E
; ---------------------------------------------------------------------------
Op2_WaitA6Script:
	tst.b	(g_A6ScriptBusy).w
	beq.w	VM2_Next
	subq.l	#$2,a2
	bra.w	Op2_Yield
; ---------------------------------------------------------------------------
; Op2_WaitA6ScriptYield0 - a2 op for coroutine 0: waits for the a6 loader script by saving this op in slot 0 ($38(a4)) and returning to the stepping script
; In:  a2 = coroutine script (no operand); (sp) = the stepping script's a2
; Out: NEXT when done; else slot 0 = this op, a2 popped, NEXT
; Evidence: move.l a2,$38(a4) / movea.l (a7)+,a2; 4 corpus runs; $48C1E `4404 095C`
; ---------------------------------------------------------------------------
Op2_WaitA6ScriptYield0:
	tst.b	(g_A6ScriptBusy).w
	beq.w	VM2_Next
	subq.l	#$2,a2
	move.l	a2,$38(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_WaitA6ScriptYield1 - a2 op for coroutine 1: waits for the a6 loader script by saving this op in slot 1 ($3C(a4))
; In:  a2 = coroutine script (no operand); (sp) = the stepping script's a2
; Out: NEXT when done; else slot 1 = this op, a2 popped, NEXT
; Evidence: move.l a2,$3C(a4); 3 corpus runs; $46FB8 `4418 0BF6`
; ---------------------------------------------------------------------------
Op2_WaitA6ScriptYield1:
	tst.b	(g_A6ScriptBusy).w
	beq.w	VM2_Next
	subq.l	#$2,a2
	move.l	a2,$3C(a4)
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
