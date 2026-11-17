; ---------------------------------------------------------------------------
; Item_SpawnExtraLifeFromHit - spawn action 28: the extra-life pickup (lives +1, sound $42) knocked out of a struck object: pops up (motion coroutine $13378) with half the hit's knockback as x velocity
; In:  d4/d5 = x/y (SpawnAction_Run passes the dying object's position)
; Out: d0 = 0; on a short pool Obj_SpawnEnemyOrQuit returns from the caller
; Evidence: Tbl_012EF0 entry 28; Scr2_0135B2: coroutine 1 = $13378 (Op2_SetCruiseY $640, Op2_JumpAnim, bounce); then Item_LaunchFromHit; 4 TAS frames (7200-115744)
; ---------------------------------------------------------------------------
Item_SpawnExtraLifeFromHit:
	lea.l	(Scr2_0135B2).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Item_LaunchFromHit(pc)
