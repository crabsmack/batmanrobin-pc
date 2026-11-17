; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverLongC - wave spawner: the third drone of the long-hover trio (Scr2_02A36A): window x +$F0..+$10E right of the player and 60-80 px below it (y +$3C..+$50) for $B40 frames, then off left; its attack loop ($2A3CA: 12 rounds of two shots) follows
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = ($50, $3C, $10E, $F0) -> $27D34; the $10E x bound wraps in Mot_StorePlayerWindow's byte packing ($10E/2 = $87 -> -$F2), so Op2_HoldPlayerWindow keeps the drone hunting round x +$F0; Scr5_021D3A +$836/+$854/+$872; TAS 39740-40416
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverLongC:
	lea.l	(Scr2_02A36A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
