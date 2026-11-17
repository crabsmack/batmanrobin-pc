; ---------------------------------------------------------------------------
; Enemy_GunTank_SpawnLow - spawns a lab gun tank as Enemy_GunTank_Spawn (cruise $80, energy $100, coroutine 0 $45DEC) but on the lower floor line $A3 and with its own fire coroutine $45E9E
; In:  d4/d5 = position (a5 wave spawn entry, VM5_SpawnEntry's jsr (a0) $007016)
; Out: jumps to Obj_SpawnEnemy with a2 = its script (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: Scr2_045E50: Enemy_GunTank_Spawn's script with PlaceYOnScreen $A3 (not $92) and coroutine 1 $45E9E; the wave spawn entries at $041402 and $04140A (x -$28 / $168, y $E070) name it; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_GunTank_SpawnLow:
	lea.l	(Scr2_045E50).l,a2
	jmp	(Obj_SpawnEnemy).l
