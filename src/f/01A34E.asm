; ---------------------------------------------------------------------------
; Boss_Harley_LobAtPlayer - Harley mid-boss native: lobs an arc projectile (a4 script Scr4_01AB76) from the pole top toward the targeted player; does nothing with no target
; In:  a6 = the Harley object, a4 = its speed block (+$10 = targeted player record or 0)
; Out: g_ArcTargetX/Y = the player record's x/y (+$2E/+$32); the effect starts at the drawn position $4E, $50 - $41 (16.16 in d4/d5, d2/d3 = 0)
; Evidence: called by op $2CB2 (CallNativeRel) at $19BAC in the Harley script; jmp Fx_RunScript; TAS f5765-f5930 (4 frames) while Harley stands on the raised pole (black balls beside her at f5850)
; ---------------------------------------------------------------------------
Boss_Harley_LobAtPlayer:
	move.w	$10(a4),d0
	beq.s	loc_01A386
	movea.w	d0,a0
	move.w	$2E(a0),(g_ArcTargetX).w
	move.w	$32(a0),(g_ArcTargetY).w
	lea.l	(Scr4_01AB76).l,a0
	moveq	#$0,d2
	moveq	#$0,d3
	moveq	#$0,d4
	moveq	#$0,d5
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	subi.w	#$41,d5
	swap	d4
	swap	d5
	jmp	(Fx_RunScript).l
loc_01A386:
	rts
