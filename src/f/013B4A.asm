; ---------------------------------------------------------------------------
; Item_SpawnPowerUpDrifting - the weapon-drop director's pickup from the right edge: drifts in sideways (coroutine $133CA) and raises the weapon level by one when touched
; In:  d4/d5 = world x/y (WeaponDrop_SpawnOnScreen)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: dc.l in the director scripts at $1402E/$14050 after WeaponDrop_FromRight; Scr2_013B56 as Item_SpawnPowerUpFalling with the drifting motion; ran in scenario-level-skip, not the TAS
; ---------------------------------------------------------------------------
Item_SpawnPowerUpDrifting:
	lea.l	(Scr2_013B56).l,a2
	jmp	(Obj_SpawnEnemy).l
