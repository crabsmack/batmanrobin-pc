; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnLoops - wave spawner: a red gun drone (Scr2_02BA8A, body $27F70, cruise $355) that runs 4 times round a 6-leg course (relative (dx, dy) (-$32, 0), the absolute point (x $D2, y $C6), relative (0, +$90), (+$32, 0), (0, -$9E), (0, -$9E)), then flies 600 px up off the screen; its attack Scr2_02BA3C fires 3 pairs of shots left $168 ticks apart, then pauses $5A0
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetCounter1 4 / Op2_LoopCounter1, final leg (dx, dy) = (0, -$258) (Op2_SetVector2HereGotoArgs2 words are (dy, dx): FDA8 0000); the 2nd leg goes to $27DA8 (Op2_AimAtArgPoint, absolute); 18 entries in Scr5_021D3A +$A2C.., Scr5_022780, Scr5_022A5E (drop actions 0/$31/8); TAS 38295-48250
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnLoops:
	lea.l	(Scr2_02BA8A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
