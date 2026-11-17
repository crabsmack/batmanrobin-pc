; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnChainRandom - spawner of a propeller toy carrying a hanging 6-figure chain (ToyChain_MakeHanging6; weight 1): it flies in left for $3C0 ticks, then holds one of four player windows picked at random (Op2_SwitchRandom; args x -$DC..-$F0 with y 0, -40 or -80; two cases cruise on again after $5A00 ticks)
; In:  d4/d5 = spawn position (spawn entry; the spawner long's top byte is a spawn action)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A5C6 / jmp Obj_SpawnEnemy; script: Op2_SetAnim (propeller frames $448E/$4498 over the toy $4484), `244A 002A 0003A73E`, `0B16 F008`, body $037E20 (`0A26`, score $32, energy $40, body box), `09F4 C000 ...`, each case Op2_CruiseLeft, timer $3C0, Op2_GosubArgs window to $037EE6; 11 spawn entries in Tbl_034142 (+$124-+$2B8, 8 of them with spawn actions $11/$17/$2E); TAS f101822-f107737 (28 spawns, section $3300 tunnel)
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnChainRandom:
	lea.l	(Scr2_03A5C6).l,a2
	jmp	(Obj_SpawnEnemy).l
