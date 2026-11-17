; ---------------------------------------------------------------------------
; Obj_SpawnEnemyAfter - Obj_SpawnEnemy with the object linked right after the running object (Obj_SpawnAfter): a 4-block script object (enemy, $58 = $A) at d4/d5 running script a2
; In:  a2 = a2-VM script, d4/d5 = x/y, a6 = the running object
; Out: d0 = 0 and a6 = the object on success; Z clear (d0 nonzero) when the pool is short
; Evidence: moveq #4,d7 / lea VM2_RunObject,a0 / bsr.w Obj_SpawnAfter / bne / move.l a2,$2A(a4) / move.w #$A,$58(a6) / moveq #0,d0, Obj_SpawnEnemy's shape; jmp target of the 12 Hatter road piece spawners ($03FB68-$03FD7E); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Obj_SpawnEnemyAfter:
	moveq	#$4,d7
	lea.l	($7D2).w,a0
	bsr.w	Obj_SpawnAfter
	bne.s	loc_008E3A
	move.l	a2,$2A(a4)
	move.w	#$A,$58(a6)
	moveq	#$0,d0
loc_008E3A:
	rts
