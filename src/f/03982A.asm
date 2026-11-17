; ---------------------------------------------------------------------------
; ToyChain_Spawn5B - spawner of a toy-chain base with a 5-link chain (child $039942) using the second charge cycle $0380D8 and the fixed-delay shedding schedule $03870C
; In:  d4/d5 = position (trigger entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_039836
; Evidence: Op2_SpawnChild $2A dc.l $039942 (dc.w 5); Tbl_034142 (+$4C4, +$4CC); TAS f110618 (1 frame)
; ---------------------------------------------------------------------------
ToyChain_Spawn5B:
	lea.l	(Scr2_039836).l,a2
	jmp	(Obj_SpawnEnemy).l
