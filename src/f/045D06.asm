; ---------------------------------------------------------------------------
; Enemy_GunTank_SpawnToughMid - spawns a tough lab gun tank (body $42FEC, floor height $92, energy $200, score $190) at a fixed speed with cruise $1AA
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_045D12: score $190, energy $200, x accel/cruise $1AA, coroutines 0 $45D6A and 1 $45D82, Goto the gun tank body $42FEC; the lab wave's spawn entry at $04173E (x $17C, y $E070) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GunTank_SpawnToughMid:
	lea.l	(Scr2_045D12).l,a2
	jmp	(Obj_SpawnEnemy).l
