; ---------------------------------------------------------------------------
; ObjPool_Free - pushes the $5A-byte block a0 back onto the object pool's free list
; In:  a0 = block
; Out: g_ObjPoolFree +1
; Evidence: 13 call sites, e.g. $53D4 frees $14(a6) after unlinking it ($8C6C), $3798 frees a list of child blocks; inverse of ObjPool_Alloc
; ---------------------------------------------------------------------------
ObjPool_Free:
	move.w	(g_ObjPoolHead).w,(a0)
	move.w	a0,(g_ObjPoolHead).w
	addq.b	#$1,(g_ObjPoolFree).w
	rts
; ObjPool_HasFree - tells whether the object pool still has at least d0 free blocks
; In:  d0 = blocks wanted
; Out: d0 = 0 (and Z set) when g_ObjPoolFree >= d0, else d0 = -1; nothing is allocated
; Evidence: $10216 calls it with d0 = 3, then ObjPool_AllocClear three times
ObjPool_HasFree:
	cmp.b	(g_ObjPoolFree).w,d0
	bhi.s	loc_00877C
	moveq	#$0,d0
	rts
loc_00877C:
	moveq	#-$1,d0
	rts
; ObjPool_Clear - zeroes the $5A bytes of the object block at a0
; In:  a0 = block
; Out: a0 unchanged, d0 = 0
; Evidence: the same 22 move.l + 1 move.w clear as ObjPool_AllocClear, without the pop; callers $1E70A, $265DC, $370DC, $37708
ObjPool_Clear:
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
