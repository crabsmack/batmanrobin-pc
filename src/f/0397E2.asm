; ---------------------------------------------------------------------------
; ToyChain_Spawn7 - spawner of a toy-chain base with a 7-link chain (child $0398FE), charge cycle $0380B8 and shedding schedule Scr2_0386CC
; In:  d4/d5 = position (wave entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0397EE
; Evidence: Op2_SpawnChild $2A dc.l $0398FE (dc.w 7); Scr5_034932 (+$252, +$25A); TAS f85421 (1 frame)
; ---------------------------------------------------------------------------
ToyChain_Spawn7:
	lea.l	(Scr2_0397EE).l,a2
	jmp	(Obj_SpawnEnemy).l
