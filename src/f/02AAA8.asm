; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnTrackYB - wave spawner: a black gun drone (Scr2_02AAB8) on the TrackY course aimed 40 px left of its entry; fires 3 rounds of two shots ($78 then $F0 apart), pause $1E0, repeat
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx) = (0, -$28) -> $27D90; Scr5_023156 +$BC (x $190, drop action $2B); a second attack loop at $2AB22 (4 rounds, then $5A00) is not reached from this spawner's script head; TAS 51018-57127
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnTrackYB:
	lea.l	(Scr2_02AAB8).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
