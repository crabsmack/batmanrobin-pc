; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnZigzagB - wave spawner: a red biplane (Scr2_02DE0E) that flies 166 px in and 56 px down, zigzags +-10 px downward while speeding up, then +-40 px with legs climbing farther each time, then loops through a long zigzag
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$2A6 (origin entry, group action 1); a never-run spawner follows inside the span at $2DF4C (lea Scr2_02DF58 / jmp Obj_SpawnEnemy, raw); fuzz runs only
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnZigzagB:
	lea.l	(Scr2_02DE0E).l,a2
	jmp	(Obj_SpawnEnemy).l
