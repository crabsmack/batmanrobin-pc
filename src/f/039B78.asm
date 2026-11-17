; ---------------------------------------------------------------------------
; ToyChain_SpawnDropIn12 - spawner of a drop-in toy chain (script Scr2_039B84, body Scr2_037B12): an invisible base that falls from above the screen onto the terrain, kicks up dust on landing and holds a 12-figure chain (ToyChain_MakeDropIn12); population weight 5
; In:  d4/d5 = spawn position (from the wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039B84 / jmp Obj_SpawnEnemy; Scr2_039B84 `244A 002A 00039BD4`, `0A32 0005` (Op2_AddPopulation), goto Scr2_037B12 (op $1CDC 0028 0180 = fall; on the floor Op2_FxSpawnOffAbs Scr4_010D9A/Scr4_010DAC + sound $B); wave entries Scr5_034932+$83E; TAS f92263-f92886 (7 spawns, section $3200 mushroom forest); png shot f92290: a figure column coming down from the top, f92330: standing on the floor
; ---------------------------------------------------------------------------
ToyChain_SpawnDropIn12:
	lea.l	(Scr2_039B84).l,a2
	jmp	(Obj_SpawnEnemy).l
