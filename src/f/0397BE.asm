; ---------------------------------------------------------------------------
; ToyChain_Spawn6 - spawner of a toy-chain base with a 6-link chain (ToyChain_MakeChild6), charge cycle $0380B8 and shedding schedule Scr2_0386CC
; In:  d4/d5 = position (wave entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0397CA
; Evidence: Op2_SpawnChild $2A dc.l ToyChain_MakeChild6; Scr5_034932 (+$242, +$24A); TAS f84217-f85120 (4 frames)
; ---------------------------------------------------------------------------
ToyChain_Spawn6:
	lea.l	(Scr2_0397CA).l,a2
	jmp	(Obj_SpawnEnemy).l
