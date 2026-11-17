; ---------------------------------------------------------------------------
; Enemy_GunTank_SpawnToughSlow - spawns a tough lab gun tank (energy $200, score $190) at a random speed ($33-$E6) with cruise $155
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_046026: as Enemy_GunTank_SpawnTough's script with x accel/cruise $155 and coroutines 0 $4604E / 1 $4605C; the lab wave's spawn entry at $04175E (x $17C, y $E070) names it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GunTank_SpawnToughSlow:
	lea.l	(Scr2_046026).l,a2
	jmp	(Obj_SpawnEnemy).l
