; ---------------------------------------------------------------------------
; ToyChain_Spawn10D - spawner of a toy-chain base on Scr2_039716, the words of ToyChain_Spawn10C's script (10-link chain, charge $038578, shedding $038788) in a copy of its own
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039716
; Evidence: the stub of ToyChain_Spawn10B; Scr2_039716 Op2_SpawnChild $2A dc.l $039A06; the wave spawn entry at $034686 (x $154, y $E09C) through VM5_SpawnEntry's jsr (a0) $007016; no corpus run at 4:3 (03970A.syn)
; ---------------------------------------------------------------------------
ToyChain_Spawn10D:
	lea.l	(Scr2_039716).l,a2
	jmp	(Obj_SpawnEnemy).l
