; ---------------------------------------------------------------------------
; ToyChain_Spawn7B - spawner of a toy-chain base with a 7-link chain (child $039986), charge cycle $0380D8, shedding schedule $03870C
; In:  d4/d5 = position (trigger entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03987E
; Evidence: Op2_SpawnChild $2A dc.l $039986 (dc.w 7); Tbl_034142 (+$4E4, +$4EC); in the corpus by ctarget
; ---------------------------------------------------------------------------
ToyChain_Spawn7B:
	lea.l	(Scr2_03987E).l,a2
	jmp	(Obj_SpawnEnemy).l
