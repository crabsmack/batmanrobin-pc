; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnNarrowUp - wave spawner: a red biplane (Scr2_02DD20) on the narrow course mirrored upward: 160 px in and 48 px up, then a +-20 px zigzag drifting up at rising speed, then off left
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$248 (x $154, y $20F8), spawned right after Enemy_Biplane_SpawnNarrowDown; TAS 45022-45113
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnNarrowUp:
	lea.l	(Scr2_02DD20).l,a2
	jmp	(Obj_SpawnEnemy).l
