; ---------------------------------------------------------------------------
; Op5_ShowGoSign - a5 op: shows the GO! sign (HUD_ShowGoSignCb), which releases the camera lock when it arrives
; In:  a5 = script (no operands)
; Out: NEXT
; Evidence: jsr HUD_ShowGoSignCb; 35 script uses, typically `6ED2 ... 6E9A 6A72` (wait until all are dead, GO!, end), e.g. $16B92; 17 corpus runs
; ---------------------------------------------------------------------------
Op5_ShowGoSign:
	jsr	(HUD_ShowGoSignCb).l
	movea.w	(a5)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op5_WaitPopulation - a5 op: waits (re-checking every frame) until g_ObjPopulation is at most n, then continues
; In:  a5 = script: word n
; Out: NEXT, or yields with the pc on this op
; Evidence: cmp.w g_ObjPopulation / bge VM5_Next / subq.l #4,a5 then the wait tail (clr $2E, save pc, restore sp); 31 script uses, e.g. $2201A (`6EA4 0009 6EEC`); 11 corpus runs
; ---------------------------------------------------------------------------
Op5_WaitPopulation:
	move.w	(a5)+,d0
	cmp.w	(g_ObjPopulation).w,d0
	bge.w	VM5_Next
	subq.l	#$4,a5
	bra.s	Op5_WaitYield
