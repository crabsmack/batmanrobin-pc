; ---------------------------------------------------------------------------
; Obj_FreeWithBlocks - returns object a6 and every nonzero block handle in its 8 words +8..+$17 to the pool free list (does not unlink it)
; In:  a6 = object
; Out: g_ObjPoolHead = the last block pushed, g_ObjPoolFree += 1 + handles
; Evidence: jmp from Obj_Destroy for unlinked objects (+4 = 0) and the fall-through of Obj_UnlinkFreeWithBlocks; $29250 frees a half-built pair with it; 8 unrolled `move.w (a0)+,d0 / beq / exg / move.w d0,(a1)` pushes; TAS 2182 frames
; ---------------------------------------------------------------------------
Obj_FreeWithBlocks:
	move.w	(g_ObjPoolHead).w,(a6)
	movea.l	a6,a1
	lea.l	$8(a6),a0
	moveq	#$1,d1
	move.w	(a0)+,d0
	beq.s	loc_008A60
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008A60:
	move.w	(a0)+,d0
	beq.s	loc_008A6C
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008A6C:
	move.w	(a0)+,d0
	beq.s	loc_008A78
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008A78:
	move.w	(a0)+,d0
	beq.s	loc_008A84
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008A84:
	move.w	(a0)+,d0
	beq.s	loc_008A90
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008A90:
	move.w	(a0)+,d0
	beq.s	loc_008A9C
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008A9C:
	move.w	(a0)+,d0
	beq.s	loc_008AA8
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008AA8:
	move.w	(a0)+,d0
	beq.s	loc_008AB4
	ext.l	d0
	exg.l	d0,a1
	move.w	d0,(a1)
	addq.b	#$1,d1
loc_008AB4:
	move.w	a1,(g_ObjPoolHead).w
	add.b	d1,(g_ObjPoolFree).w
	rts
