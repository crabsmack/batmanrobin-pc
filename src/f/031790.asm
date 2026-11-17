; ---------------------------------------------------------------------------
; Enemy_BallTurret_SpawnVolley - spawns a ball turret on the hull that turns fast (rate $1000) toward the targeted player and every $3C0 fires a volley of 3 aimed shots (Fx_SpawnAimedN, a4 $3184A) $140 apart, then swaps target
; In:  d4/d5 = trigger x/y; a1 = the entry's extra word (start delay, timer 0)
; Out: as Spawn_SetTimer0FromEntry; pops one return address when the pool is short
; Evidence: 5 trigger entries in Tbl_0307C2 (+$65C-$6D2); body $2FDD4: energy $150, score $96, map-anchored, aims each frame (Op2_AimAtTracked, Op2_TurnHeading); frames $3FE8-$401A rendered: a riveted ball with a barrel turning; TAS 67920-74707 (3)
; ---------------------------------------------------------------------------
Enemy_BallTurret_SpawnVolley:
	lea.l	(Scr2_0317A2).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Spawn_SetTimer0FromEntry).l
