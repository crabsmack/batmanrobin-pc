; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverC - wave spawner: the third drone of the hover trio (Scr2_02A1D6): windows x +$F0..+$10E right of the player and 60-80 px below it (y +$3C..+$50), then x +$B4..+$D2 and 60-80 px above it (y -$50..-$3C), $780 frames each, then off left
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = ($50, $3C, $10E, $F0) and (-$3C, -$50, $D2, $B4) -> $27D34; the $10E x bound wraps in Mot_StorePlayerWindow's byte packing ($10E/2 = $87 -> -$F2), so Op2_HoldPlayerWindow keeps the drone hunting round x +$F0; spawned with HoverA/HoverB (Scr5_021D3A +$6D0, +$76A..; Scr5_023B38 +$FE..); the trio's shared attack loop is at $2A23C in this script; TAS 35553-37821
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverC:
	lea.l	(Scr2_02A1D6).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
