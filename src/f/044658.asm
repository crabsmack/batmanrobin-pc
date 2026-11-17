; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn3B - spawns a hall hover pod on course 3 ($44892) with attack B ($44E14, bursts of seven shots), the fastest x speed $300
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_044664: x accel/cruise $300, y $100, coroutine 0 $44892, coroutine 1 $44E14; wave entries Tbl_041138 +$F6, +$19E, +$214; TAS 117579-118668 (2 spawns)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn3B:
	lea.l	(Scr2_044664).l,a2
	jmp	(Obj_SpawnEnemy).l
