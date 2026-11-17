; ---------------------------------------------------------------------------
; Obj_SpawnAfter - allocates a d7-block object with handler a0 linked right after the running object (Obj_NewMultiAfter) and initialises it at d4/d5 (Obj_InitSpawned)
; In:  a0 = handler, d7 = blocks, d4/d5 = x/y, a6 = the running object
; Out: d0 = 0, a6 = the object, a3-a5 its blocks; the pool short: d0 = -1, Z clear
; Evidence: movem.l a1-a2,-(a7) / bsr.w Obj_NewMultiAfter / movem.l (a7)+,a1-a2 / beq.w Obj_InitSpawned / rts, Obj_SpawnUnlinked's shape; bsr target of Obj_SpawnEnemyAfter $008E28 (and the raw $008E3C, $008EC4, $008EDE); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Obj_SpawnAfter:
	movem.l	a1-a2,-(a7)
	bsr.w	Obj_NewMultiAfter
	movem.l	(a7)+,a1-a2
	beq.w	Obj_InitSpawned
	rts
