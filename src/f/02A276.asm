; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverLongA - wave spawner: the first of a faster trio of gun drones (Scr2_02A286, entry cruise x $355) that hold one window beside the player, x +$B4..+$D2 right of it and level with it (y -$14..+$14), for $B40 frames, then fly off left
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = ($14, -$14, $D2, $B4) -> $27D34; Scr5_021D3A +$822/+$840/+$85E with HoverLongB/C (x $14A, delays $1E0/$3C0/$5A0); TAS 39651-40326
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverLongA:
	lea.l	(Scr2_02A286).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
