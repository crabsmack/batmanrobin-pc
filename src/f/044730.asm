; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn3C - spawns a hall hover pod on course 3 ($44892) with attack C ($44E82, eight spaced shots), x speed $200
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04473C: coroutine 0 $44892, coroutine 1 $44E82; wave entries Tbl_041138 +$FE, +$1A6, +$21C; TAS frame 117760
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn3C:
	lea.l	(Scr2_04473C).l,a2
	jmp	(Obj_SpawnEnemy).l
