; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverLongB - wave spawner: the second drone of the long-hover trio (Scr2_02A2FA): window x +$F0..+$10E right of the player and 60-80 px above it (y -$50..-$3C) for $B40 frames, then off left
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = (-$3C, -$50, $10E, $F0) -> $27D34; the $10E x bound wraps in Mot_StorePlayerWindow's byte packing ($10E/2 = $87 -> -$F2), so Op2_HoldPlayerWindow keeps the drone hunting round x +$F0; Scr5_021D3A +$82C/+$84A/+$868; TAS 39695-40372
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverLongB:
	lea.l	(Scr2_02A2FA).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
