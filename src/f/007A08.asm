; ---------------------------------------------------------------------------
; Spr_ResetList - clears the per-frame sprite display list: 80 free entries, empty depth-sorted list, empty front list, SAT buffer write pointer at its start
; In:  none
; Out: g_SprNodesFree ($F4AE) = 80, node allocator $9DDC = $A3A4 (grows down), front list end $9DDE = $9E16 (grows up), head $9DE0 linked to the tail sentinel $9DF2 (next = $FFFF), g_SATPtr = $9B56, g_SprCount = 0
; Evidence: called at every frame start ($9B86, $9BAC); the Spr_Add* functions consume what it sets up and Spr_BuildSAT walks it
; ---------------------------------------------------------------------------
Spr_ResetList:
	move.b	#$50,(g_SprNodesFree).w
	move.w	#$A3A4,(g_SprNodeAlloc).w
	move.w	#$9E16,(g_SprFrontEnd).w
	lea.l	(g_SprSortHead).w,a0
	lea.l	($FFFF9DF2).w,a1
	move.w	a1,$2(a0)
	move.w	a1,$6(a0)
	move.w	#$9DE6,$4(a0)
	clr.w	(a1)
	move.w	#$FFFF,$6(a1)
	move.w	#$9E04,$4(a1)
	move.w	#$9B56,(g_SATPtr).w
	clr.w	(g_SprCount).w
	rts
