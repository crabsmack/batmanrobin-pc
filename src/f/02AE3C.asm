; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnCircuitA - wave spawner: a black gun drone (Scr2_02AE4C, body $27DBE) on the same circuit as Enemy_RedDrone_SpawnCircuit (30 px left, 2 laps through the absolute points (x $3C, y $13C), (x $78, y $50), (x $50, y $13C), (x $3C, y $50), 4 random down-left exits, 40 px left, track the player's height $F00) but cruising faster ($3AA, exit $400); fires through the shared attack $2B078
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Scr5_023156 +$1EA/+$1F4 (x $B4, y $1FC4, delays $F0/$870, group action 1); attack $2B078: 6 x two shot requests (gosub $27E2C = Op2_IncVar0 unless a player is in front) $F0 apart, then $2D0 pause; TAS 49667-49818
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnCircuitA:
	lea.l	(Scr2_02AE4C).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
