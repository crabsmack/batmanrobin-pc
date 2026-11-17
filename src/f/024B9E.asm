; ---------------------------------------------------------------------------
; Enemy_BarrelThug_SpawnD - lift trigger callback: spawns barrel thug variant Scr2_024BAA (drop action $24, timer $1E0, then the random-wait cycle)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $0213C8 and $021A0C in Tbl_02105A; body $024CC8; TAS f28291-f31437
; ---------------------------------------------------------------------------
Enemy_BarrelThug_SpawnD:
	lea.l	(Scr2_024BAA).l,a2
	jmp	(Obj_SpawnEnemy).l
