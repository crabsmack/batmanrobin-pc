; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnTrackY - wave spawner: a red gun drone (Scr2_02A9A6, body $27F70: energy $88) that flies in toward a point 10 px left of its entry (cruise x $200, y $300), stops, follows the player's height for $F00 frames and leaves; fires two shots left from (dx, dy) = (-$10, -$B/+$B) every $140 frames
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs2 words are (dy, dx) = (0, -$A) -> $27D90 (Op2_AimAtTargetArgOffset after Op2_TargetSelf); 7 entries in Scr5_023156 (x $154, drop actions $31/$1A/$2A/$2B/0); vector 2 $27D1A (Op2_CruiseToPlayerY until timer 2); shots via Fx_SpawnAtOfsAngle ($9010: heading $200, script $2CF92); frames $3A02-$3A16 rendered: the drone body in red/orange; TAS 42282-57038
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnTrackY:
	lea.l	(Scr2_02A9A6).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
