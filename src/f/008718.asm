; ---------------------------------------------------------------------------
; ObjPool_AllocClear - pops one block off the object pool and zeroes all $5A bytes of it
; In:  none
; Out: d0 = 0 and a0 = the cleared block, or d0 = -1 when the pool is empty
; Evidence: $101CE checks ObjPool_HasFree(3) and then calls it three times for an entity's a3/a4/a5 parts; also $41F0E; same pop as ObjPool_Alloc plus 22 move.l + 1 move.w of 0
; ---------------------------------------------------------------------------
ObjPool_AllocClear:
	subq.b	#$1,(g_ObjPoolFree).w
	bcs.s	loc_00875C
	movea.w	(g_ObjPoolHead).w,a0
	move.w	(a0),(g_ObjPoolHead).w
	moveq	#$0,d0
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.l	d0,(a0)+
	move.w	d0,(a0)+
	lea.l	-$5A(a0),a0
	rts
loc_00875C:
	clr.b	(g_ObjPoolFree).w
	moveq	#-$1,d0
	rts
