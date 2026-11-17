; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SpawnHoverLeft - spawns a tube-section jetpack trooper that holds a window left of the targeted player (x -$50..-$B4), cruise $1AA, turn rate $3AA
; In:  d4/d5 = x/y (wave entry, x $168)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_0454C2: coroutine 0 $454E8 with Op2_GosubArgs ($50, -$50, -$50, -$B4), waits $B40/$780; wave entry $41CB4; RAM f123660/f127740 slot 0 $45500; TAS 123611-127731 (2 spawns)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SpawnHoverLeft:
	lea.l	(Scr2_0454C2).l,a2
	jmp	(Obj_SpawnEnemy).l
