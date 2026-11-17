; ---------------------------------------------------------------------------
; Item_BoxPlayer - the player record of collision box a1's owner (owner id $58 = 1 or 2); otherwise pops the return address so the calling native ends
; In:  a1 = collision box (+$2E owner object)
; Out: a0 = g_Player1 / g_Player2; a1 = owner
; Evidence: fall-through of Item_FoundBoxPlayer; also called by Drop_CollectHealth with the box in a2; 277 TAS frames
; ---------------------------------------------------------------------------
Item_BoxPlayer:
	movea.w	$2E(a1),a1
	cmpi.w	#$1,$58(a1)
	beq.s	loc_012FDC
	cmpi.w	#$2,$58(a1)
	beq.s	loc_012FD4
	addq.l	#$4,a7
	rts
loc_012FD4:
	lea.l	(g_Player2).l,a0
	rts
loc_012FDC:
	lea.l	(g_Player1).l,a0
	rts
