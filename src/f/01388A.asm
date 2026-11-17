; ---------------------------------------------------------------------------
; Item_SpawnRapidChargeFromHit - spawn action 22: the rapid-charge pickup (fast weapon charge for $2D0 frames, sound $48) knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x velocity
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 22; Scr2_01389A: coroutine 1 = $13378 (Op2_SetCruiseY $640, Op2_JumpAnim, bounce); then Item_LaunchFromHit; 10 TAS frames (6969-108864)
; ---------------------------------------------------------------------------
Item_SpawnRapidChargeFromHit:
	lea.l	(Scr2_01389A).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
