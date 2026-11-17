; ---------------------------------------------------------------------------
; Enemy_VentTank_SpawnRandomSpeed - spawns a lab vent tank (body $42D74, floor height $9E, energy $300) at a random speed ($33-$E6), cruise $D5, its first vent after $3C0 ticks
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_045938: speed $33-$E6, score $C8, energy $300, x accel/cruise $D5, coroutine 0 $4595C (gosub $42DEA), Goto the vent tank body $42D74; the lab wave's spawn entries at $0417A8 and $0417B0 (x -$28 / $168, y $E070) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_VentTank_SpawnRandomSpeed:
	lea.l	(Scr2_045938).l,a2
	jmp	(Obj_SpawnEnemy).l
