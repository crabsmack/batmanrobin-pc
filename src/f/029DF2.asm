; ---------------------------------------------------------------------------
; Enemy_Jet_SpawnMissileLauncherB - wave spawner: a rocket jet like Enemy_Jet_SpawnMissileLauncher's (Scr2_029DFE, the same body) whose hold time is one of $2D0/$3C0/$5A0/$690 at random, without its screen-x range test
; In:  d4/d5 = x/y (the wave entry's)
; Out: as Obj_SpawnEnemy (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: spawner long(s) at 12 wave entries $023976-$023A80; lea script / jmp Obj_SpawnEnemy as its siblings; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Enemy_Jet_SpawnMissileLauncherB:
	lea.l	(Scr2_029DFE).l,a2
	jmp	(Obj_SpawnEnemy).l
