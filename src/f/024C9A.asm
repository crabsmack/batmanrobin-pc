; ---------------------------------------------------------------------------
; Enemy_GreenThug_SpawnB - lift trigger callback: green-suited thug (no barrel) variant Scr2_024CA6 on the body entry $024CD8 (as Enemy_GreenThug_SpawnA; drop action $24) that cycles only the steps $024E34 (face the targeted player) and $024EC0 (jump, op $1C8C) every $1E0
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $02123A, $021296, $021300 ... $02194A in Tbl_02105A; TAS f22959-f30352 (23 spawns, the most common)
; ---------------------------------------------------------------------------
Enemy_GreenThug_SpawnB:
	lea.l	(Scr2_024CA6).l,a2
	jmp	(Obj_SpawnEnemy).l
