; ---------------------------------------------------------------------------
; ToyChain_Spawn6B - spawner of a toy-chain base with a 6-link chain (child $039964), charge cycle $0380D8, shedding schedule $03870C
; In:  d4/d5 = position (trigger entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_03985A
; Evidence: Op2_SpawnChild $2A dc.l $039964 (dc.w 6); Tbl_034142 (+$4D4, +$4DC); in the corpus by ctarget, no TAS function-start frames
; ---------------------------------------------------------------------------
ToyChain_Spawn6B:
	lea.l	(Scr2_03985A).l,a2
	jmp	(Obj_SpawnEnemy).l
