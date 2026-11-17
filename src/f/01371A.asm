; ---------------------------------------------------------------------------
; Item_SpawnBombFromHit - spawn action 25: the bomb pickup (clears the screen, sound $10) knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x velocity
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 25; Scr2_01372A: coroutine 1 = $13378 (Op2_SetCruiseY $640, Op2_JumpAnim, bounce); then Item_LaunchFromHit; 12 TAS frames (14616-118546)
; ---------------------------------------------------------------------------
Item_SpawnBombFromHit:
	lea.l	(Scr2_01372A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
