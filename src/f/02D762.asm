; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnWeave - wave spawner: a red biplane (Scr2_02D76E) that weaves through a long course of 120-px legs with rising speed ($400-$600), then a +-40 px zigzag and a 600-px exit, and loops
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$2F0/+$328/+$350 (origin entries, group action 1); TAS 36399-42273 (30 frames)
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnWeave:
	lea.l	(Scr2_02D76E).l,a2
	jmp	(Obj_SpawnEnemy).l
