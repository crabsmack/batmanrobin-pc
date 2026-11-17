; ---------------------------------------------------------------------------
; Obj_SpawnBoss - spawns a 4-block script object with $58 = $B (boss class) at d4/d5 running script a2
; In:  a2 = script, d4/d5 = x/y
; Out: d0 = 0, a6 = the object; Z clear on failure
; Evidence: 16 callers, mostly run once per stage: $1805A on TAS f4518 just before the stage 1 boss fight (f6000 shows the boss and its gauge; its objects have $58 = $B); $39AC scales class $B energy by $F64C instead of $F64A (boss role inferred)
; ---------------------------------------------------------------------------
Obj_SpawnBoss:
	moveq	#$4,d7
; ---------------------------------------------------------------------------
; Obj_SpawnBossN - Obj_SpawnBoss with the block count in d7
; In:  d7 = blocks, a2 = script, d4/d5 = x/y
; Out: as Obj_SpawnBoss
; Evidence: caller $1C3F2
; ---------------------------------------------------------------------------
Obj_SpawnBossN:
	lea.l	($7D2).w,a0
	bsr.w	Obj_Spawn
	bne.s	loc_008E70
	move.l	a2,$2A(a4)
	move.w	#$B,$58(a6)
	moveq	#$0,d0
loc_008E70:
	rts
