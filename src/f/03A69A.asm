; ---------------------------------------------------------------------------
; Enemy_PropToy_SpawnFlyBy - spawner of a lone propeller toy (no chain; weight 1) that flies across to the left (speed $33-$E6, cruise x $100); placed with drop spawn actions, so it carries an item or health drop
; In:  d4/d5 = spawn position (trigger/spawn entry; the callback long's top byte = spawn action $0F/$10/$16/$19/$27)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A6A6 / jmp Obj_SpawnEnemy; script SetAnim, `0B16 F008`, goto $037E20, coroutine 0: Op2_SetCruiseX $100, Op2_CruiseLeft; entries Tbl_033F30+$3E/+$F6/+$140/+$178 and Tbl_034142+$162/+$170/+$17E/+$18C ...; TAS f90435-f110376 (13 spawns, sections $3200-$3400); level-skip scenario f9364; png shot f90480: the fat toy with a propeller cap flying over the mushroom forest
; ---------------------------------------------------------------------------
Enemy_PropToy_SpawnFlyBy:
	lea.l	(Scr2_03A6A6).l,a2
	jmp	(Obj_SpawnEnemy).l
