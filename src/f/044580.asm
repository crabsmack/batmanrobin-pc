; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn3A - spawns a hall hover pod on course 3 ($44892: a second pair of waypoint subroutines) with attack A ($44D68, rounds of six aimed shots), x speed $200
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04458C: coroutine 0 $44892, coroutine 1 $44D68, x $200, y $155; wave entries Tbl_041138 +$EE, +$196, +$20C; TAS 118543-118791 (2 spawns)
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn3A:
	lea.l	(Scr2_04458C).l,a2
	jmp	(Obj_SpawnEnemy).l
