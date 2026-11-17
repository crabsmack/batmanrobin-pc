; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn1B - spawns a hall hover pod on course 1 ($44836) with attack B ($44E14: three bursts of seven shots $30 ticks apart), slower y speed $100
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_0445D4: x $2AA, y accel/cruise $100, coroutine 0 $44836, coroutine 1 $44E14; wave entries Tbl_041138 +$76, +$AA, +$16E, +$1E4; 2 fuzz-117000 runs only (f118993, f119175)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn1B:
	lea.l	(Scr2_0445D4).l,a2
	jmp	(Obj_SpawnEnemy).l
