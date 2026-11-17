; ---------------------------------------------------------------------------
; Hitbox_InitLists - empties both collision box lists: links g_PlayerBoxList to g_PlayerBoxTail and g_EnemyBoxList to g_EnemyBoxTail, each tail's next = 0
; In:  none
; Out: both lists empty
; Evidence: only caller the common scene init $9AAC (after ObjPool_Init); node links +0 next, +2 prev, as Hitbox_LinkPlayerList/EnemyList and Hitbox_Unlink use them; TAS 25 frames
; ---------------------------------------------------------------------------
Hitbox_InitLists:
	lea.l	(g_PlayerBoxList).w,a0
	lea.l	(g_PlayerBoxTail).w,a1
	move.w	a1,(a0)
	move.w	a0,$2(a1)
	clr.w	(a1)
	lea.l	(g_EnemyBoxList).w,a0
	lea.l	(g_EnemyBoxTail).w,a1
	move.w	a1,(a0)
	move.w	a0,$2(a1)
	clr.w	(a1)
	rts
