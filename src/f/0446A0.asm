; ---------------------------------------------------------------------------
; Enemy_HoverPod_Spawn1C - spawns a hall hover pod on course 1 ($44836) with attack C ($44E82: eight shots $3C ticks apart, then a long pause), x $2AA, y $155
; In:  d4/d5 = x/y (wave entry, screen x $A0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_0446AC: coroutine 0 $44836, coroutine 1 $44E82 (turn rate $100, counter 8, Op2_FxSpawnRegs); wave entries Tbl_041138 +$7E, +$B2, +$176, +$1EC; TAS frame 117215
; ---------------------------------------------------------------------------
Enemy_HoverPod_Spawn1C:
	lea.l	(Scr2_0446AC).l,a2
	jmp	(Obj_SpawnEnemy).l
