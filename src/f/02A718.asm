; ---------------------------------------------------------------------------
; Enemy_BlackDrone_SpawnHoverRandomA - wave spawner: a black gun drone (Scr2_02A728, cruise $300) that holds for $F00 frames one of three windows right of the targeted player at random (x +$D2..+$DC: level y -$14..+$14, above y -$50..-$3C or below y +$3C..+$50), then leaves
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0230D8, $023100, $02311E, $02313C; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_BlackDrone_SpawnHoverRandomA:
	lea.l	(Scr2_02A728).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
