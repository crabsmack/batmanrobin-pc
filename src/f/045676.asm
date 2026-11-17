; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnAbove - spawns a tube-section jetpack trooper (random speed) that alternates between windows above-right and above-left of the targeted player (y words -$5A/-$64)
; In:  d4/d5 = x/y (wave entry, x $168 or -$28)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045682: coroutine 0 $456AA: Op2_GosubArgs (-$5A, -$64, $A0, $50) / (-$5A, -$64, -$50, -$A0), turn rate $300; wave entries $41CE4/$41CEC; RAM f125220-f125880 slot 0 $456C2-$456E2 at y 29-65 (high on the screen); TAS 125212-128188 (4 spawns)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnAbove:
	lea.l	(Scr2_045682).l,a2
	jmp	(Obj_SpawnEnemy).l
