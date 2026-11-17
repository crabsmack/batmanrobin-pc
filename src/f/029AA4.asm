; ---------------------------------------------------------------------------
; Enemy_SpinMine_SpawnUp - spawns the spinning mine with Scr2_029AB0, identical to Enemy_SpinMine_SpawnDown except that its first point is 20 px above the launch point
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: dc.l in the a2 subroutine $299B8 (gosub'd by $2D024, $2E2A8, $2E5A6); the two scripts differ only in the first Op2_SetVector2HereGotoArgs2 offset (0, -$14) vs (0, +$14); fuzz-43200 / fuzz-77400 only
; ---------------------------------------------------------------------------
Enemy_SpinMine_SpawnUp:
	lea.l	(Scr2_029AB0).l,a2
	jmp	(Obj_SpawnEnemy).l
