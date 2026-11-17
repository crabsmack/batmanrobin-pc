; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnHoverOnFast - spawns a tube-section jetpack trooper that hovers in a window on the targeted player (args $50,-$50,$50,-$50) as Enemy_JetTrooper_SpawnHoverOn but at cruise $1AA, re-aiming after $3C0 ticks
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_0455FE: Enemy_JetTrooper_SpawnHoverOn's script (gosub $439C6, turn rate 0 then $500, body $423CE) with accel/cruise $1AA and timer $3C0; the tube wave's spawn entries at $041CD4 and $041CDC (x -$28 / $168, y $E070) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnHoverOnFast:
	lea.l	(Scr2_0455FE).l,a2
	jmp	(Obj_SpawnEnemy).l
