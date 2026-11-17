; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverB - wave spawner: the second drone of the hover trio (Scr2_02A156): holds a player window nominally 240-270 px right (x +$F0..+$10E; the $10E bound wraps, see Evidence) and 60-80 px above it (y -$50..-$3C), then the same x band at the player's height (y -$14..+$14), $780 frames each, then off left
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word
; Out: as Enemy_BlackDrone_SpawnWaypointsA
; Evidence: Op2_SetVector2HereGotoArgs4 words (y hi, y lo, x hi, x lo) = (-$3C, -$50, $10E, $F0) and ($14, -$14, $10E, $F0) -> Op2_SetPlayerWindowFromArgs (Arg0/1 = x lo/hi, Arg2/3 = y lo/hi); spawned with HoverA/HoverC (Scr5_021D3A +$6C6, +$760..; Scr5_023B38 +$F4..); TAS 35493-37761; x hi $10E packs to byte $87 (Mot_StorePlayerWindow stores half of each bound) and reads back as -$F2, so the drone hunts round x +$F0 instead of holding the 240-270 px band (NOTES)
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverB:
	lea.l	(Scr2_02A156).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
