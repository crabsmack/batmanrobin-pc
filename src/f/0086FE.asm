; ---------------------------------------------------------------------------
; ObjPool_Alloc - pops one $5A-byte block off the object pool's free list without clearing it
; In:  none
; Out: d0 = 0 and a0 = block, or d0 = -1 when the pool is empty (g_ObjPoolFree clamps at 0)
; Evidence: callers $120E, $3798 (fills $30(a0) = script $82C, $2E(a0) = parent a6), $385A, $3B76, $4F4E, $66E2, $6EBA, $37260; ObjPool_Free pushes blocks back
; ---------------------------------------------------------------------------
ObjPool_Alloc:
	subq.b	#$1,(g_ObjPoolFree).w
	bcs.s	loc_008710
	movea.w	(g_ObjPoolHead).w,a0
	move.w	(a0),(g_ObjPoolHead).w
	moveq	#$0,d0
	rts
loc_008710:
	clr.b	(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
