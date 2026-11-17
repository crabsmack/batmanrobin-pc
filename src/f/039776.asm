; ---------------------------------------------------------------------------
; ToyChain_Spawn7C - spawner of a toy-chain base carrying a slow-swaying 7-link chain (ToyChain_MakeChild7C) with charge cycle $0385B4 and shedding schedule $0385F0
; In:  d4/d5 = position (wave spawn entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039782
; Evidence: the stub of ToyChain_Spawn5; Scr2_039782 Op2_SpawnChild $2A dc.l $039A66; the wave spawn entries at $035014 (x -$28) and $03501C (x $168, y $E0C2) through VM5_SpawnEntry's jsr (a0) $007016; no corpus run at 4:3 (039776.syn)
; ---------------------------------------------------------------------------
ToyChain_Spawn7C:
	lea.l	(Scr2_039782).l,a2
	jmp	(Obj_SpawnEnemy).l
