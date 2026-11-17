; ---------------------------------------------------------------------------
; Enemy_BarrelThug_SpawnE - barrel thug variant Scr2_024BEC (drop action $25, first move after timer $528, a fixed $780 cycle timer where the others draw $3C0-$780 at random)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $0219FC (Tbl_02105A) and $021D02 (wave script Tbl_021B26); body $024CC8; TAS f28300-f30260
; ---------------------------------------------------------------------------
Enemy_BarrelThug_SpawnE:
	lea.l	(Scr2_024BEC).l,a2
	jmp	(Obj_SpawnEnemy).l
