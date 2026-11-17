; ---------------------------------------------------------------------------
; ToyChain_Spawn8 - spawner of a toy-chain base with an 8-link chain (child $039920), charge cycle $0380B8 and shedding schedule Scr2_0386CC
; In:  d4/d5 = position (wave entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039812
; Evidence: Op2_SpawnChild $2A dc.l $039920 (dc.w 8); Scr5_034932+$262; TAS f86023 (1 frame)
; ---------------------------------------------------------------------------
ToyChain_Spawn8:
	lea.l	(Scr2_039812).l,a2
	jmp	(Obj_SpawnEnemy).l
