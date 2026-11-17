; ---------------------------------------------------------------------------
; Enemy_RedDrone_SpawnHoverRight - wave spawner: Enemy_RedDrone_SpawnHoverOnPlayer's red drone (Scr2_02B7E6, random speed) holding its $1E00-tick window right of the targeted player (x +$B4..+$C8, y -$A..+$A), not on it
; In:  d4/d5 = x/y (the wave entry's), a1 = the entry
; Out: Obj_SpawnEnemyOrQuit's results (a short pool returns from the caller), then the tail's
; Evidence: spawner long(s) at $0220B0, $02225E; lea script / jsr Obj_SpawnEnemyOrQuit / jmp Spawn_SetTimer1FromEntry(pc) as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_RedDrone_SpawnHoverRight:
	lea.l	(Scr2_02B7E6).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Spawn_SetTimer1FromEntry(pc)
