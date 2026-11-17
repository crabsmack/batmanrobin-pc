; ---------------------------------------------------------------------------
; Enemy_Gunman_SpawnDropIn - spawns the trench-coat gunman in palette line 3 (energy $75) that drops in from above before taking position
; In:  d4/d5 = x/y (wave script Tbl_0174F0 +$69A..+$6AA)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01BFC4: SetEnergy $75, coroutine 0 = $1C17C (op $1CDC fall $46,$140, anim $AE, floor check); gunmen wait above the screen (y < 0) in RAM f2220-f2940; shot f2460 (gunman firing on the awning, tagged Scr2_01BFC4); TAS f13277-f15671
; ---------------------------------------------------------------------------
Enemy_Gunman_SpawnDropIn:
	lea.l	(Scr2_01BFC4).l,a2
	jmp	(Obj_SpawnEnemy).l
