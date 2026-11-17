; ---------------------------------------------------------------------------
; Enemy_BarrelThug_SpawnB - lift trigger callback: spawns barrel thug variant Scr2_024B24 (drop action $24, first move after timer $5A0)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $0219F4 in Tbl_02105A; same body $024CC8 as Enemy_BarrelThug_SpawnA; TAS f27317-f31022 (7 spawns)
; ---------------------------------------------------------------------------
Enemy_BarrelThug_SpawnB:
	lea.l	(Scr2_024B24).l,a2
	jmp	(Obj_SpawnEnemy).l
