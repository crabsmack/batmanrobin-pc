; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnHoverOn - spawns a jetpack trooper of the stage 4 tube section that hovers in a window on the targeted player (args $50,-$50,$50,-$50), turn rate 0 then $500, slow cruise $155
; In:  d4/d5 = x/y (wave entry: x $168 right edge, y $70)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045460: speed $100, population +1, score $1F4, drop init $439C6, energy $1B0/$1E0 (1P/2P), coroutine 0 $45486 (Op2_GosubArgs into Op2_SetPlayerWindowFromArgs $424B0, waits $780), cruise $155, body $423CE; wave entries $41CAC, $41D04 (tube wave Scr2_0417CC) and $41710; RAM f125700 an object of this body at (193, 29) = the white armoured trooper with a jetpack and a gun (shot w29_c125700); scenario-level-skip f13292, fuzz runs
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnHoverOn:
	lea.l	(Scr2_045460).l,a2
	jmp	(Obj_SpawnEnemy).l
