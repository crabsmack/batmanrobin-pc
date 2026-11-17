; ---------------------------------------------------------------------------
; Obj_SpawnBossOrQuit - spawns a 4-block $58 = $B script object, returning to the caller's caller when the pool is short
; In:  a2 = script, d4/d5 = x/y
; Out: a6 = the object; on failure pops one return address
; Evidence: callers $13F02, $2EA1E, $3DA2E; 11 TAS frames
; ---------------------------------------------------------------------------
Obj_SpawnBossOrQuit:
	moveq	#$4,d7
	lea.l	($7D2).w,a0
	bsr.w	Obj_Spawn
	bne.s	loc_008E8A
	move.l	a2,$2A(a4)
	move.w	#$B,$58(a6)
	rts
loc_008E8A:
	addq.l	#$4,a7
	rts
; ---------------------------------------------------------------------------
; Obj_SpawnBossUnlinked - spawns a 4-block $58 = $B script object without linking it on the main object list
; In:  a2 = script, d4/d5 = x/y
; Out: d0 = 0, a6 = the object; Z clear on failure
; Evidence: callers $260F2, $26158, $2F038, $3B3B8, $3B91E (24 sites)
; ---------------------------------------------------------------------------
Obj_SpawnBossUnlinked:
	moveq	#$4,d7
	lea.l	($7D2).w,a0
	bsr.w	Obj_SpawnUnlinked
	bne.s	loc_008EA6
	move.l	a2,$2A(a4)
	move.w	#$B,$58(a6)
	moveq	#$0,d0
loc_008EA6:
	rts
