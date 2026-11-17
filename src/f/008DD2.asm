; ---------------------------------------------------------------------------
; Obj_SpawnEnemyNOrQuit - Obj_SpawnEnemyOrQuit with the block count in d7
; In:  d7 = blocks, a2 = script, d4/d5 = x/y
; Out: a6 = the object; on failure returns to the caller's caller
; Evidence: callers $2658E, $2785E, $278B2, $278CE; `addq.l #4,a7 / rts` on failure
; ---------------------------------------------------------------------------
Obj_SpawnEnemyNOrQuit:
	lea.l	($7D2).w,a0
	bsr.w	Obj_Spawn
	bne.s	loc_008DE8
	move.l	a2,$2A(a4)
	move.w	#$A,$58(a6)
	rts
loc_008DE8:
	addq.l	#$4,a7
	rts
; ---------------------------------------------------------------------------
; Obj_SpawnEnemyUnlinked - spawns a 4-block enemy script object without linking it on the main object list
; In:  a2 = script, d4/d5 = x/y
; Out: d0 = 0, a6 = the object; Z clear on failure
; Evidence: uses Obj_SpawnUnlinked ($8984 allocates without the list insert); callers $327FE, $32AAA, $32AC2, $32D4E
; ---------------------------------------------------------------------------
Obj_SpawnEnemyUnlinked:
	moveq	#$4,d7
	lea.l	($7D2).w,a0
	bsr.w	Obj_SpawnUnlinked
	bne.s	loc_008E04
	move.l	a2,$2A(a4)
	move.w	#$A,$58(a6)
	moveq	#$0,d0
loc_008E04:
	rts
; ---------------------------------------------------------------------------
; Obj_SpawnEnemyUnlinkedOrQuit - Obj_SpawnEnemyUnlinked that returns to the caller's caller when the pool is short
; In:  a2 = script, d4/d5 = x/y
; Out: a6 = the object; on failure pops one return address
; Evidence: caller $3DB50
; ---------------------------------------------------------------------------
Obj_SpawnEnemyUnlinkedOrQuit:
	moveq	#$4,d7
	lea.l	($7D2).w,a0
	bsr.w	Obj_SpawnUnlinked
	bne.s	loc_008E1E
	move.l	a2,$2A(a4)
	move.w	#$A,$58(a6)
	rts
loc_008E1E:
	addq.l	#$4,a7
	rts
