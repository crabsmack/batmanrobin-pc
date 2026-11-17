; ---------------------------------------------------------------------------
; ToyRing_SpawnBigB - spawner (5 blocks) of a 16-figure toy ring (radius $6E, stiffness $40, as ToyRing_SpawnBig) with energy $200 and score $12C (ToyRing_SpawnBig: $400, $3E8), its own coroutines $03958C / $0395D2 / $039630
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemyN with a2 = Scr2_039028, d7 = 5
; Evidence: the stub of its sibling ToyRing_SpawnBig; called by the wave spawn entry at $03491E (x $14A, y $E070) through VM5_SpawnEntry's jsr (a0) $007016; ToyRing_Init words $10 $6E $40 3 $CCCC; no corpus run
; ---------------------------------------------------------------------------
ToyRing_SpawnBigB:
	lea.l	(Scr2_039028).l,a2
	move.w	#$5,d7
	jmp	(Obj_SpawnEnemyN).l
