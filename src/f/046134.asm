; ---------------------------------------------------------------------------
; Enemy_BombBot_SpawnHall - spawns a bomb bot in the stage 4 hall (floor height $B6, cruise $1AA, fuse timer words $B40/$B40) with a drop picked at spawn
; In:  d4/d5 = x/y (wave entry: x $154 or -$14)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_046140: population +1, drop init gosub $43A68 (Op2_BranchWeaponLvlMaxGE + Op2_SwitchRandom -> Op2_SetSpawnAction $15/$09/$12 ...), score $C8, energy $100, body $432E2; sibling $04616A has no drop init; wave entries Tbl_041138 +$4C/+$54 and Scr5_0413AC +$C/+$14; RAM f117480 an object of this body at x 85 (camera 61): the antenna capsule robot of shot w29_c117480; TAS 116432-118602 (7 spawns)
; ---------------------------------------------------------------------------
Enemy_BombBot_SpawnHall:
	lea.l	(Scr2_046140).l,a2
	jmp	(Obj_SpawnEnemy).l
