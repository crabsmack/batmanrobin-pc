; ---------------------------------------------------------------------------
; ToyRing_SpawnSixB - spawner (5 blocks) of a six-figure toy ring (radius $32, stiffness $60), energy $C0, no drop action; it stops, hops in place, then rolls in bouts of 3 hops at cruise x $2AA (coroutine 2 $0393CA) and splits into single figures later (coroutine 1 $039612)
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_0391BA, d7 = 5
; Evidence: the sibling spawners' stub (ToyRing_SpawnRandom, ToyRing_SpawnSix); called by the wave spawn entries at $0344B4, $0344BC, $034CCC through VM5_SpawnEntry's jsr (a0) $007016; Scr2_0391BA = Scr2_0391F4 less Op2_SetSpawnAction $25, coroutine 2 $0393CA for $03941E; no 4:3 corpus run; widescreen_gameplay=1 reaches it (scenario-level-skip sync 8391)
; ---------------------------------------------------------------------------
ToyRing_SpawnSixB:
	lea.l	(Scr2_0391BA).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
