; ---------------------------------------------------------------------------
; ToyChain_Spawn10E - spawner of a toy-chain base carrying a 10-link chain (ToyChain_MakeChild10) with ToyChain_Spawn10A's charge cycle $038596 and the shedding schedule $0387CC
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03973A
; Evidence: the stub of ToyChain_Spawn10B; Scr2_03973A Op2_SpawnChild $2A dc.l $039A06; the wave spawn entry at $03468E (x -$14, y $E09C) through VM5_SpawnEntry's jsr (a0) $007016; no corpus run at 4:3 (03972E.syn)
; ---------------------------------------------------------------------------
ToyChain_Spawn10E:
	lea.l	(Scr2_03973A).l,a2
	jmp	(Obj_SpawnEnemy).l
