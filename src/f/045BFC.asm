; ---------------------------------------------------------------------------
; Enemy_GunTank_SpawnHeavy - spawns a lab gun tank (body $42FEC, floor height $92) with energy $300 and score $190 that cruises at $22A
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_045C08: score $190, energy $300, x accel/cruise $22A, coroutines 0 $45C2E and 1 $45C46, Goto the gun tank body $42FEC; the lab wave's spawn entry at $0416BC (x $154, y $E070) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GunTank_SpawnHeavy:
	lea.l	(Scr2_045C08).l,a2
	jmp	(Obj_SpawnEnemy).l
