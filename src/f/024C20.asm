; ---------------------------------------------------------------------------
; Enemy_BarrelThug_SpawnF - barrel thug variant Scr2_024C2C (drop action $24, first move after timer $7F8)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $021A04 (Tbl_02105A), $021CFA/$021D0A (Tbl_021B26); body $024CC8; TAS f28309-f30261
; ---------------------------------------------------------------------------
Enemy_BarrelThug_SpawnF:
	lea.l	(Scr2_024C2C).l,a2
	jmp	(Obj_SpawnEnemy).l
