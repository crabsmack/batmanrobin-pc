; ---------------------------------------------------------------------------
; Enemy_Jet_SpawnMissileLauncher - wave spawner: a rocket jet (Scr2_029D18, same body and random pickup) that flies in, follows the player's height, then fires a fan of 4 homing missiles and U-turns away
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: 36 entries in Scr5_022C7E (+$2E2..) and Tbl_022FA4; four GosubArgs ($29D84-$29DA2: headings $240/$1C0/$40/$3C0) into $29DAE, which calls Spawn_AtOwnOfs with Enemy_Missile_SpawnFromJet unless a player is within $1E px; exit $2865A (heading + $200, no body box); TAS 43060-62428
; ---------------------------------------------------------------------------
Enemy_Jet_SpawnMissileLauncher:
	lea.l	(Scr2_029D18).l,a2
	jmp	(Obj_SpawnEnemy).l
