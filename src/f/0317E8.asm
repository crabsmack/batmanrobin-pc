; ---------------------------------------------------------------------------
; Enemy_BallTurret_SpawnStream - spawns a ball turret that turns slowly (rate $400) and, while on screen, fires a stream of 10 aimed shots $F0 apart, then stops turning and waits $960
; In:  d4/d5 = trigger x/y; a1 = the entry's extra word (start delay, timer 0)
; Out: as Spawn_SetTimer0FromEntry; pops one return address when the pool is short
; Evidence: 4 trigger entries in Tbl_0307C2 (+$678-$6B4); body $2FDD4; TAS 71513-74279 (2)
; ---------------------------------------------------------------------------
Enemy_BallTurret_SpawnStream:
	lea.l	(Scr2_0317FA).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	(Spawn_SetTimer0FromEntry).l
