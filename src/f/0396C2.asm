; ---------------------------------------------------------------------------
; ToyChain_Spawn10B - spawner of a toy-chain base carrying a 10-link chain (child $039A06) with charge cycle $038578 and shedding schedule $038796
; In:  d4/d5 = position (wave entry)
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0396CE
; Evidence: Scr2_0396CE: Op2_SpawnChild $2A dc.l $039A06; wave script Scr5_034932+$2A4; TAS f86799 (1 frame)
; ---------------------------------------------------------------------------
ToyChain_Spawn10B:
	lea.l	(Scr2_0396CE).l,a2
	jmp	(Obj_SpawnEnemy).l
