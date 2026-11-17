; ---------------------------------------------------------------------------
; Enemy_VentTank_SpawnFastest - spawns a lab vent tank (body $42D74, floor height $9E, energy $300) that cruises at $2AA, vents $F00 ticks between $780-tick rests
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_0458CC: score $C8, energy $300, x accel/cruise $2AA, coroutine 0 $458EE, Goto the vent tank body $42D74; the lab wave's spawn entry at $0417A0 (x $168, y $E070) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_VentTank_SpawnFastest:
	lea.l	(Scr2_0458CC).l,a2
	jmp	(Obj_SpawnEnemy).l
