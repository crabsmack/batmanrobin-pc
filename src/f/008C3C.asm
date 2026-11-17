; ---------------------------------------------------------------------------
; Hitbox_LinkPlayerList - appends collision box a0 to the end of g_PlayerBoxList (after g_PlayerBoxLast, before g_PlayerBoxTail)
; In:  a0 = box (pool block)
; Out: a0 +0 next = g_PlayerBoxTail, +2 prev = the old last box; g_PlayerBoxLast = a0; a1 preserved (usp)
; Evidence: bra from Hitbox_LinkByKind for kinds 0/4 (Op2_PlaceBodyBoxes); Hitbox_FindPlayerBox scans the list; TAS 73 frames
; ---------------------------------------------------------------------------
Hitbox_LinkPlayerList:
	move.l	a1,usp
	movea.w	(g_PlayerBoxLast).w,a1
	move.w	a0,(g_PlayerBoxLast).w
	move.w	a0,(a1)
	move.w	a1,$2(a0)
	move.w	#$DEB0,(a0)
	move.l	usp,a1
	rts
