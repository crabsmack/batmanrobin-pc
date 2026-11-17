; ---------------------------------------------------------------------------
; Item_SpawnBigHealthFromHit - spawn action 31: the large health pickup (+$80 health, sound $41) knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x velocity
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 31; Scr2_013402: coroutine 1 = $13378 (Op2_SetCruiseY $640, Op2_JumpAnim, bounce); then Item_LaunchFromHit; 7 TAS frames (7258-80494)
; ---------------------------------------------------------------------------
Item_SpawnBigHealthFromHit:
	lea.l	(Scr2_013402).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
