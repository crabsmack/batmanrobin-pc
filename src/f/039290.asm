; ---------------------------------------------------------------------------
; ToyRing_SpawnSixC - spawner (5 blocks) of a six-figure toy ring of stiffness $80 (radius $32), energy $C0, no drop action; its own movement coroutine ($03952C) and coroutine 3 $0380A8, splitting into single figures later (coroutine 1 $039612)
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_0392A0, d7 = 5
; Evidence: the sibling spawners' stub (ToyRing_SpawnSix, ToyRing_SpawnSixB); called by the wave spawn entries at $034CBC and $034CC4 (the wave holding ToyRing_SpawnSixB's $034CCC) through VM5_SpawnEntry's jsr (a0) $007016; ToyRing_Init words 6 $32 $80 1 $9248; no corpus run
; ---------------------------------------------------------------------------
ToyRing_SpawnSixC:
	lea.l	(Scr2_0392A0).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
