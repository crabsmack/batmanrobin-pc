; ---------------------------------------------------------------------------
; Enemy_BombDropper_Spawn - trigger callback of the bomb dropper (script Scr2_036050, energy $40, score $C8): a box hanging on the overhead pipe; once the camera brings it to the screen x given by the entry it waits the entry delay, then drops bouncing bombs at the targeted player
; In:  d4/d5 = trigger x/y; a1 = the entry's extra words: target screen x (-> a3 +$14), delay (-> timer 0)
; Out: d0 = 0 (or the caller's caller on a full pool: Obj_SpawnEnemyOrQuit)
; Evidence: dc.l entries in the trigger table Tbl_033CBE ($033D48-$033E7A, section $3100 $3FE94); TAS f79360-f86599 (25 spawns) at y 19 (the pipe); png f079560 the red box under the pipe
; ---------------------------------------------------------------------------
Enemy_BombDropper_Spawn:
	lea.l	(Scr2_036050).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	move.w	(a1)+,$14(a3)
	move.w	(a1)+,$1A(a4)
	moveq	#$0,d0
	rts
