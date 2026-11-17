; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnBelow - spawns a tube-section jetpack trooper (random speed) that alternates between windows below-right and below-left of the targeted player (y words $64/$5A)
; In:  d4/d5 = x/y (wave entry, x $168 or -$28)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045702: coroutine 0 $4572A: Op2_GosubArgs ($64, $5A, $A0, $50) / ($64, $5A, -$50, -$A0); wave entries $41CF4/$41CFC; RAM f125460/f125880/f128100 slot 0 $45742 at y 198-200 (low on the screen); TAS 125392-128368 (4 spawns)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnBelow:
	lea.l	(Scr2_045702).l,a2
	jmp	(Obj_SpawnEnemy).l
