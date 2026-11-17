; ---------------------------------------------------------------------------
; Enemy_Clown_SpawnRanges - spawns a standard-palette clown (palette line 3, energy $80) with Enemy_Clown_SpawnGreen's script: drops in and picks one of three approach ranges at random
; In:  d4/d5 = x/y (the trigger entry's)
; Out: as Obj_SpawnEnemy
; Evidence: trigger callback longs at $016F26 and $016F2E (Tbl_0166EA, the parade-truck section $1400); Scr2_01B760 = Scr2_01B69A's words but Op2_ModSprAttr $9860 (line 3, not $9840); lea / jmp Obj_SpawnEnemy; raw until crane-crash (2026-10-30): no corpus run reaches it
; ---------------------------------------------------------------------------
Enemy_Clown_SpawnRanges:
	lea.l	(Scr2_01B760).l,a2
	jmp	(Obj_SpawnEnemy).l
