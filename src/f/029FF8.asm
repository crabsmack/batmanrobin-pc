; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverA - wave spawner: the first of a trio of gun drones (Scr2_02A008) that each hold two windows beside the targeted player for $780 frames apiece (this one: x +$B4..+$D2 right of the player at its height (y -$14..+$14), then x +$F0..+$10E and 60-80 px below it (y +$3C..+$50)), then fly off left
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: entries in Scr5_021D3A (+$6BC, +$756..) and Scr5_023B38 (+$EA..), always with HoverB/HoverC at x $154 and delays $1E0/$4B0/$780; window op $27D34 (Op2_SetPlayerWindowFromArgs + Op2_HoldPlayerWindow until timer 2), Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = ($14, -$14, $D2, $B4) and ($50, $3C, $10E, $F0); the $10E x bound wraps in Mot_StorePlayerWindow's byte packing ($10E/2 = $87 -> -$F2), so Op2_HoldPlayerWindow keeps the drone hunting round x +$F0; attack loop $2A23C (two shots, $F0/$168 apart, then $2D0); TAS 35434-36156
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverA:
	lea.l	(Scr2_02A008).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
