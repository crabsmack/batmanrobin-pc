; ---------------------------------------------------------------------------
; ToyChain_Spawn8B - spawner of a toy-chain base carrying an 8-link chain (ToyChain_MakeChild8B) on ToyChain_Spawn5B-7B's fixed-delay schedules (charge $0380D8, shedding $03870C)
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0398A2
; Evidence: the stub of ToyChain_Spawn7B; Scr2_0398A2 Op2_SpawnChild $2A dc.l $0399A6; the wave spawn entry at $034632 (x $154, y $E09C) through VM5_SpawnEntry's jsr (a0) $007016; no corpus run at 4:3 (039896.syn)
; ---------------------------------------------------------------------------
ToyChain_Spawn8B:
	lea.l	(Scr2_0398A2).l,a2
	jmp	(Obj_SpawnEnemy).l
