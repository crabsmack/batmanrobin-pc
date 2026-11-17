; ---------------------------------------------------------------------------
; Obj_Spawn - allocates a d7-block object with handler a0, links it on the main object list and initialises it at d4/d5
; In:  a0 = handler, d7 = blocks, d4/d5 = x/y
; Out: d0 = 0, a6 = object, a3/a4/a5 = its extra blocks; Z clear (d0 = -1) when the pool is short
; Evidence: $8912 takes d7 pool blocks and links the first before g_ObjListTail; then Obj_InitSpawned; used by every Obj_SpawnEnemy*/Boss* entry and $1E70A; 1903 TAS frames
; ---------------------------------------------------------------------------
Obj_Spawn:
	movem.l	a1-a2,-(a7)
	bsr.w	Obj_NewMulti
	movem.l	(a7)+,a1-a2
	beq.s	Obj_InitSpawned
	rts
