; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChainHover - spawner of a propeller toy (weight 5) with a hanging 6-figure chain (ToyChain_MakeHanging6) that holds a window 40-80 px above the targeted player, after $2580 ticks drifts left in it and later leaves
; In:  d4/d5 = spawn position (spawn entry with a spawn action)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A6E2 / jmp Obj_SpawnEnemy; `244A 002A 0003A73E`, goto $037E26 (`0A32 0005`), `2D00 FFD8 FFB0 0000 0000` -> $037EE6 (Op2_SetPlayerWindowFromArgs), timers $2580 / $2940 + gosub $037F0A (window + Op2_CruiseLeft); spawn entries Tbl_034142+$14C/+$236/+$2C8 (actions $2E/$11); TAS f101686-f107917 (12 spawns, section $3300); level-skip scenario f9585
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChainHover:
	lea.l	(Scr2_03A6E2).l,a2
	jmp	(Obj_SpawnEnemy).l
