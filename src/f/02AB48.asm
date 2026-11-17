; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnTrackYC - wave spawner: a black gun drone (Scr2_02AB58) on the TrackY course aimed 10 px left of its entry, with TrackYB's attack (3 rounds of two shots, pause $1E0)
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx) = (0, -$A) -> $27D90; Scr5_023156 +$B2 (drop $2A), +$E6, +$F0; TAS 51018-57127
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnTrackYC:
	lea.l	(Scr2_02AB58).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
