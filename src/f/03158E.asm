; ---------------------------------------------------------------------------
; Enemy_MissileDome_SpawnSingles - spawns a missile dome on the hull that opens its hatch (anim $2FCB0) and three times, when on screen, launches one Enemy_Missile_SpawnAgile to the left (GosubArgs to $3164E: heading $200, 5 frames straight, $F homing), then closes (anim $2FD4A) and waits $F00
; In:  d4/d5 = trigger x/y; a1 = the entry's extra word (timer 1)
; Out: as Spawn_SetTimer1FromEntry; pops one return address when the pool is short
; Evidence: 2 trigger entries (Tbl_0307C2 +$62C/+$63C); body $2FC56: energy $150/$300 (1P/2P), score $64, attr palette 1 + H flip; frames $3F5C-$3FD4 rendered: a dome whose shell opens, $3FDE a launcher rack; TAS f68764
; ---------------------------------------------------------------------------
Enemy_MissileDome_SpawnSingles:
	lea.l	(Scr2_0315A0).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Spawn_SetTimer1FromEntry).l
