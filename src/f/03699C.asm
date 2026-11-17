; ---------------------------------------------------------------------------
; Enemy_DropCarrier_SpawnRabbits - spawner (Obj_SpawnEnemy) of a drop carrier that crosses at screen height $84 and drops white-rabbit hoppers (spawner $035CF2) below itself (Scr2_0369A8: energy $100, cruise x $300, then the shared carrier body $035AA6)
; In:  d4/d5 = position (spawn entry), a1 = entry
; Out: jumps to Obj_SpawnEnemy with a2 = Scr2_0369A8
; Evidence: coroutine 0 $036B22: Op2_CallNative Spawn_AtOwnOfs dc.l $035CF2, (0, 5) every $5A0/$1680 ticks; stage 3 trigger table Tbl_034142 (+$5D0, +$666, +$744); TAS f112595-f112729 (2 frames); the dropped objects are the rabbits seen at f111900 (shot)
; ---------------------------------------------------------------------------
Enemy_DropCarrier_SpawnRabbits:
	lea.l	(Scr2_0369A8).l,a2
	jmp	(Obj_SpawnEnemy).l
