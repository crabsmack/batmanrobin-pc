; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnZigzagUp - wave spawner: the mirror of Enemy_Biplane_SpawnZigzagDown (Scr2_02D98A): the zigzag legs climb 4 px each
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$1A8/+$216/+$27E; the two scripts differ only in the sign of the per-leg y words (4 / -4); TAS 47934-66212
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnZigzagUp:
	lea.l	(Scr2_02D98A).l,a2
	jmp	(Obj_SpawnEnemy).l
