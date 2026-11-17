; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnNarrowDown - wave spawner: a red biplane (Scr2_02DA82) that jumps into the second half of Enemy_Biplane_SpawnWideUp's script ($2DC42): 160 px in and 48 px down, then a narrow +-20 px zigzag drifting 6 px per leg at rising speed, then off left
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_02DA82 is 16 bytes (vector 0 = $2DC42); Scr5_023C78 +$240 (x $154, y $2094), paired with Enemy_Biplane_SpawnNarrowUp; TAS 45013-45103
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnNarrowDown:
	lea.l	(Scr2_02DA82).l,a2
	jmp	(Obj_SpawnEnemy).l
