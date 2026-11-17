; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnTrackYA - wave spawner: a black gun drone (Scr2_02AA38) on the TrackY course (cruise $300 both axes to 10 px left of its entry, follow the player's height $F00 frames, leave); fires 4 rounds $F0 apart once
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_TargetSelf, then Op2_SetVector2HereGotoArgs2 `0000 FFF6` (words y, x -> Op2_AimAtTargetArgOffset $27D90: dx -$A, dy 0), then $27D1A (Op2_CruiseToPlayerY) for timer 2 $F00; Scr5_023156 +$5E/+$68/+$72 (drop action $31); its final wait is $4B00 (no second volley); TAS 42373-43016
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnTrackYA:
	lea.l	(Scr2_02AA38).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
