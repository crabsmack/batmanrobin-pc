; ---------------------------------------------------------------------------
; Enemy_GunTank_Spawn - spawns the lab's gun tank (a domed tracked tank with a twin-barrel turret) at floor height $92: drives in at cruise $80, stops after $1E0 ticks and fires 5-shot bursts ($50 ticks apart, every $5A0) from the turret aimed at the player
; In:  d4/d5 = x/y (trigger entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045DC2: score $C8, energy $100, x $80, gosub $43092 (turret coroutine 3: Op2_AimAtTracked + Enemy_GunTank_ClampAimUp), coroutine 0 $45DEC, coroutine 1 $45E10 (turn rate $AAA, counter 5, Op2_FxHeadingSnap $10 + Op2_FxSpawnRegs), body $42FEC (population +3, turret in the extra sprite block $10(a6)); Tbl_041000 trigger entries x 832-2624 (7, spawn action $12); RAM f119700 two objects of this body, shot w29_c119760; TAS 119573-120149 (7 spawns)
; ---------------------------------------------------------------------------
Enemy_GunTank_Spawn:
	lea.l	(Scr2_045DC2).l,a2
	jmp	(Obj_SpawnEnemy).l
