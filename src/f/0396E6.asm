; ---------------------------------------------------------------------------
; ToyChain_Spawn10C - spawner of a toy-chain base carrying a 10-link chain (ToyChain_MakeChild10) with ToyChain_Spawn10B's charge cycle $038578 and its own shedding schedule $038788
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0396F2
; Evidence: the stub of ToyChain_Spawn10B; Scr2_0396F2: Op2_SpawnChild $2A dc.l $039A06; the wave spawn entry at $034912 (x $14A, y $E098) names it through VM5_SpawnEntry's jsr (a0) $007016; no corpus run at 4:3 (0396E6.syn)
; ---------------------------------------------------------------------------
ToyChain_Spawn10C:
	lea.l	(Scr2_0396F2).l,a2
	jmp	(Obj_SpawnEnemy).l
