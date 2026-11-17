; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnHoverRight - spawns a tube-section jetpack trooper that holds a window right of the targeted player (x $50..$B4), cruise $1AA, turn rate $3AA
; In:  d4/d5 = x/y (wave entry, x -$28)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_045520: coroutine 0 $45546 with Op2_GosubArgs ($50, -$50, $B4, $50); wave entry $41CBC (enters from the left edge); TAS 123702-127822 (2 spawns)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnHoverRight:
	lea.l	(Scr2_045520).l,a2
	jmp	(Obj_SpawnEnemy).l
