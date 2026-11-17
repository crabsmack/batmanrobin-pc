; ---------------------------------------------------------------------------
; ToyChain_Spawn10A - spawner of a toy-chain base carrying a 10-link chain (child $039A06, table $039AFC) with charge cycle $038596 and shedding schedule $0387A4
; In:  d4/d5 = position (trigger or wave entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0396AA (Op2_SpawnChild, child coroutines 3/0, then the base loop Scr2_0379BC)
; Evidence: Scr2_0396AA: Op2_SpawnChild $2A dc.l $039A06 (ToyChain_SpawnInline, dc.w $A); Tbl_034142 (+$504, +$50C), Scr5_034932+$29C; TAS f86799 (1 frame)
; ---------------------------------------------------------------------------
ToyChain_Spawn10A:
	lea.l	(Scr2_0396AA).l,a2
	jmp	(Obj_SpawnEnemy).l
