; ---------------------------------------------------------------------------
; Item_SpawnPowerUpFalling - the weapon-drop director's pickup from above: falls in (coroutine $1336A, Op2_SetCruiseY $640) and raises the weapon level by one when touched; its icon value cycles every $60 ticks
; In:  d4/d5 = world x/y (WeaponDrop_SpawnOnScreen)
; Out: d0 = 0 and a6 = the item, nonzero when the pool is short
; Evidence: dc.l in the director scripts at $13FE4/$1400C/$14072 after WeaponDrop_FromTop; Scr2_013B34: touch coroutine $13C4E (sound $47, the level-up native $13D18), coroutine 0 $13C2E (Item_WeaponItemNextValue + Item_WeaponItemShowNext every $60); ran in scenario-continue/game-over, not the TAS
; ---------------------------------------------------------------------------
Item_SpawnPowerUpFalling:
	lea.l	(Scr2_013B34).l,a2
	jmp	(Obj_SpawnEnemy).l
