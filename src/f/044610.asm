; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn2B - spawns a hall hover pod on course 2 ($4487A) with attack B ($44E14, bursts of seven shots), x speed $2AA with the short accel distance $50
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04461C: x accel $2AA/$50, y $100, coroutine 0 $4487A, coroutine 1 $44E14; wave entries Tbl_041138 +$DE, +$186, +$1FC; TAS frame 118546
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn2B:
	lea.l	(Scr2_04461C).l,a2
	jmp	(Obj_SpawnEnemy).l
