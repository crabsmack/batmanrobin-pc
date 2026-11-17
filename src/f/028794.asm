; ---------------------------------------------------------------------------
; Enemy_Missile_InitFromArgs - spawner tail for the homing missile (body Scr2_0287B0): heading = g_VM2Arg0 (target and current, a3 +$A/+$C), script var 0 = Arg1 (frames flying straight before homing), var 1 = Arg2 (frames homing)
; In:  a3/a4 = the new missile's blocks (after Obj_SpawnEnemyOrQuit); g_VM2Arg0-2 set by the caller's GosubArgs op
; Out: d0 = 0
; Evidence: jumped to by 11 spawners ($29DCC, $2CE7E-$2CFCC, $2F514, $2F53C, $3165E-$31720); Scr2_0287B0 counts var 0 down (Op2_DecVar0Loop) before Op2_AimAtTracked, then var 1; frames $3A84.. rendered: a small rocket with a flame; TAS 43185-76015
; ---------------------------------------------------------------------------
Enemy_Missile_InitFromArgs:
	move.w	(g_VM2Arg0).w,d0
	move.w	d0,$A(a3)
	move.w	d0,$C(a3)
	move.w	(g_VM2Arg1).w,$22(a4)
	move.w	(g_VM2Arg2).w,$24(a4)
	moveq	#$0,d0
	rts
