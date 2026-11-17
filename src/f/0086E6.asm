; ---------------------------------------------------------------------------
; ObjPool_Init - links d0 blocks of $5A bytes at a0 into the object pool's free list
; In:  a0 = pool start, d0 = block count
; Out: g_ObjPoolFree ($F4B0) = d0, g_ObjPoolHead ($AD52) = a0; each block's first word points to the next
; Evidence: only caller scene init $9AAC with a0 = $FFAD54, d0 = $8C (140 blocks); ran on 25 TAS frames (scene loads)
; ---------------------------------------------------------------------------
ObjPool_Init:
	move.b	d0,(g_ObjPoolFree).w
	move.w	a0,(g_ObjPoolHead).w
	bra.s	loc_0086F8
loc_0086F0:
	lea.l	$5A(a0),a0
	move.w	a0,-$5A(a0)
loc_0086F8:
	dbra	d0,loc_0086F0
	rts
