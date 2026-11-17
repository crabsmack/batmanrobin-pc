; ---------------------------------------------------------------------------
; Enemy_Jet_SpawnLane - wave spawner: a rocket jet (Scr2_02B68A, body $285DE) with a random pickup ($29B76) that flies 40 px further in (cruise $2AA), then follows the player's height for $5A00 ticks and leaves; one per screen lane
; In:  d4/d5 = x/y; a1 = the entry's attack-delay word (timer 1)
; Out: as Obj_SpawnEnemyOrQuit, then Spawn_SetTimer1FromEntry
; Evidence: 5 entries Tbl_023764 +$158..+$180 (x $14A, screen y $0C/$3E/$70/$A2/$D4, drop action $2B, delay $7F80 = the attack (pairs of $27E2C shot requests every $140) practically never starts); TAS 54046-64339 (117 frames), f055200 shows rows of rocket jets
; ---------------------------------------------------------------------------
Enemy_Jet_SpawnLane:
	lea.l	(Scr2_02B68A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
