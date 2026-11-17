; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChain10 - spawner of a propeller toy (weight 5) with a hanging 10-figure chain (ToyChain_MakeHanging10): holds a player window (cruise $100/$100) for $2580 ticks, then flies off toward x -$64
; In:  d4/d5 = spawn position (spawn entry with action $11)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A778 / jmp Obj_SpawnEnemy; `244A 002A 0003A7C8`, goto $037E26, `2D00 FFD8 FF9C 0000 0000` -> $037EE6, timer $2580, `2D08 0000 FF9C` -> $037EC2 (Op2_AimAtArgPoint, Arg0 = x -$64); spawn entries Tbl_034142+$23E/+$2D0; not in the TAS: fuzz-104400-s1/s2 f106141-f107945 (section $3300)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChain10:
	lea.l	(Scr2_03A778).l,a2
	jmp	(Obj_SpawnEnemy).l
