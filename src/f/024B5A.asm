; ---------------------------------------------------------------------------
; Enemy_BarrelThug_SpawnC - lift trigger callback: spawns barrel thug variant Scr2_024B66 (drop action $25, first move after a random $F0-$1E0 wait)
; In:  d4/d5 = trigger x/y
; Out: d0 = 0 when spawned
; Evidence: dc.l at $02139E and $0219D4-$0219EC in Tbl_02105A; body $024CC8; TAS f28037-f30860
; ---------------------------------------------------------------------------
Enemy_BarrelThug_SpawnC:
	lea.l	(Scr2_024B66).l,a2
	jmp	(Obj_SpawnEnemy).l
