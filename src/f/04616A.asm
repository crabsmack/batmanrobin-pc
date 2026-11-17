; ---------------------------------------------------------------------------
; Enemy_BombBot_SpawnHallNoDrop - spawns a hall bomb bot (the capsule robot, body $0432E2) like Enemy_BombBot_SpawnHall but without its drop-init gosub: rolls in at cruise x $1AA on the floor line y $B6 with a fuse (timer 0) of $B40 + random [0, $B40)
; In:  d4/d5 = spawn position (wave spawn entry: x $154 or -$14)
; Out: jumps to Obj_SpawnEnemy (a2 = Scr2_046176)
; Evidence: Scr2_046176: IncPopulation, score $C8, energy $100, SetCruiseX $1AA, PlaceYOnScreen $B6, SetTimer0Rand $B40 $B40, Goto $0432E2 (anim uses Enemy_BombBot_FrameByCameraX); Scr2_046140 (Enemy_BombBot_SpawnHall) is the same after a Gosub; spawn entries at $041174/$04117C; TAS 116192-118360 (4 spawns), the robot at screen x 241 in a shot of f118440
; ---------------------------------------------------------------------------
Enemy_BombBot_SpawnHallNoDrop:
	lea.l	(Scr2_046176).l,a2
	jmp	(Obj_SpawnEnemy).l
