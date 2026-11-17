; ---------------------------------------------------------------------------
; ToyChain_Spawn5 - spawner of a toy-chain base with a 5-link chain (ToyChain_MakeChild5), standard charge cycle $0380B8 and the random-delay shedding schedule Scr2_0386CC
; In:  d4/d5 = position (wave entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0397A6 (base loop Scr2_0379BC: syncs and runs the chain, ends when it is gone)
; Evidence: Op2_SpawnChild $2A dc.l ToyChain_MakeChild5, Op2_StartChildCoroutine3Rel/0Rel; Scr5_034932 (+$232, +$23A); TAS f83916-f86444 (3 frames); the chain on screen at f84000/f84600
; ---------------------------------------------------------------------------
ToyChain_Spawn5:
	lea.l	(Scr2_0397A6).l,a2
	jmp	(Obj_SpawnEnemy).l
