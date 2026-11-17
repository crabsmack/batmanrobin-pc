; ---------------------------------------------------------------------------
; Obj_SpawnUnlinked - allocates a d7-block object with handler a0 (not linked on the main object list) and initialises it at d4/d5
; In:  a0 = handler, d7 = blocks, d4/d5 = x/y
; Out: d0 = 0, a6 = object, a3-a5 = its blocks; Z clear when the pool is short
; Evidence: $8984 = $8912 without the list insert; callers the Unlinked spawners and $370DC (the inline spawn $37134 links objects itself)
; ---------------------------------------------------------------------------
Obj_SpawnUnlinked:
	movem.l	a1-a2,-(a7)
	bsr.w	Obj_NewMultiUnlinked
	movem.l	(a7)+,a1-a2
	beq.w	Obj_InitSpawned
	rts
