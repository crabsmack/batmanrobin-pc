; ---------------------------------------------------------------------------
; Hitbox_FindEnemyBox - finds the first box on g_EnemyBoxList overlapping the rectangle d4-d6 x d5-d7
; In:  d4 left, d5 top, d6 right, d7 bottom
; Out: as Hitbox_FindPlayerBox: Z clear, d0 = -1, a2 = box on a hit
; Evidence: callers $3768, $4E42, $662C, $69AE; TAS f3000 the list holds the enemies' boxes (owners run VM2_RunObject with $58 = $A)
; ---------------------------------------------------------------------------
Hitbox_FindEnemyBox:
	movea.w	(g_EnemyBoxList).w,a2
	tst.w	(a2)
	bne.s	Hitbox_TestBox
	rts
