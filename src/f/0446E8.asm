; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn2C - spawns a hall hover pod on course 2 ($4487A) with attack C ($44E82, eight spaced shots), x speed $200
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_0446F4: x $200/$60, y $155, coroutine 0 $4487A, coroutine 1 $44E82; wave entries Tbl_041138 +$E6, +$18E, +$204; TAS frame 118671
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn2C:
	lea.l	(Scr2_0446F4).l,a2
	jmp	(Obj_SpawnEnemy).l
