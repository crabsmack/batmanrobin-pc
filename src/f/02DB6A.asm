; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnWideUp - wave spawner: the mirror of Enemy_Biplane_SpawnWideDown (Scr2_02DB76: 48 px up, the wide zigzag drifting up); the script's tail $2DC42 is the narrow course entered by Enemy_Biplane_SpawnNarrowDown
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$198/+$206 (x $154, y $20F8); TAS 37975-38055
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnWideUp:
	lea.l	(Scr2_02DB76).l,a2
	jmp	(Obj_SpawnEnemy).l
