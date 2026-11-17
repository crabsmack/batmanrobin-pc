; ---------------------------------------------------------------------------
; Enemy_DomeCraft_SpawnC - wave spawner: Enemy_DomeCraft_SpawnA's green-domed craft (Scr2_02D414, body $280EC) holding its windows (first y -$14..+$14, x +$78..+$C8) for $9600 ticks, not $1680
; In:  d4/d5 = x/y (the wave entry's)
; Out: as Obj_SpawnEnemy (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: spawner long(s) at $0221E6; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_DomeCraft_SpawnC:
	lea.l	(Scr2_02D414).l,a2
	jmp	(Obj_SpawnEnemy).l
