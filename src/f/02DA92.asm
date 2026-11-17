; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnWideDown - wave spawner: a red biplane (Scr2_02DA9E) that flies 160 px in and 48 px down, then zigzags +-40 px drifting down, then +-60 px drifting back up, and leaves left
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$190/+$1FE (x $154, y $2094), first of a 4-plane formation with WideUp, ZigzagDown and ZigzagUp; TAS 41152-41232
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnWideDown:
	lea.l	(Scr2_02DA9E).l,a2
	jmp	(Obj_SpawnEnemy).l
