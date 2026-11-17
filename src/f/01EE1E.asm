; ---------------------------------------------------------------------------
; Boss_HarleyJet_SpawnSecond - claw-script native: puts a flat floor at y $D2 and spawns jetpack Harley again (Scr2_01EE34: same energy, timer $5A0, skips the population wait)
; In:  d4/d5 = position from Spawn_AtCameraOfs (op $2CBC at $1E956, offset $15E/$46)
; Out: as Obj_SpawnBoss
; Evidence: Coll_SetFlatFloor #$D2; called from Scr2_01E87E +$DE; TAS f10670, the object at $B6D2 runs Scr2_01EE64 from f10740
; ---------------------------------------------------------------------------
Boss_HarleyJet_SpawnSecond:
	move.w	#$D2,d0
	jsr	(Coll_SetFlatFloor).l
	lea.l	(Scr2_01EE34).l,a2
	jmp	(Obj_SpawnBoss).l
