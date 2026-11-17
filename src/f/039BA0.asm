; ---------------------------------------------------------------------------
; ToyChain_SpawnDropIn9 - spawner of a drop-in toy chain of 9 figures (ToyChain_MakeDropIn9) on the same falling base Scr2_037B12; population weight 1 (Op2_IncPopulation)
; In:  d4/d5 = spawn position
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039BAC / jmp Obj_SpawnEnemy; Scr2_039BAC `244A 002A 00039BE2`, `0A26`, goto Scr2_037B12; wave entries Scr5_034932+$86A; TAS f93331-f93388 (section $3200). A third such spawner at $039B50 (12 links via $039BC6, energy $20, weight $A) is never run, still raw
; ---------------------------------------------------------------------------
ToyChain_SpawnDropIn9:
	lea.l	(Scr2_039BAC).l,a2
	jmp	(Obj_SpawnEnemy).l
