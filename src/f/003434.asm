; ---------------------------------------------------------------------------
; Op2_BranchPlayerInBox - a2 op: branches when the targeted player's collision box (box id = record +$3E) overlaps a rectangle given relative to the object, mirrored by its H/V flip bits
; In:  a2 = script: words left, top, right, bottom (relative to $3E/$42(a6)), word offset
; Out: branch when Hitbox_FindPlayerBoxOfId finds it, else NEXT after the operands; no target skips all 10 bytes
; Evidence: movem.w (a0)+,d4-d7, neg/exg on bits 3/4 of $38(a5), bsr Hitbox_FindPlayerBoxOfId; 43 runs; $1B09A `3434 FF6A FF79 0096 FFA1 023C`
; ---------------------------------------------------------------------------
Op2_BranchPlayerInBox:
	move.w	$10(a4),d0
	beq.s	loc_003488
	movea.w	d0,a0
	move.w	$3E(a0),d3
	move.w	$3E(a6),d1
	move.w	$42(a6),d2
	movea.l	a2,a0
	movem.w	(a0)+,d4-d7
	btst	#$3,$38(a5)
	beq.s	loc_00345C
	neg.w	d4
	neg.w	d6
	exg.l	d4,d6
loc_00345C:
	btst	#$4,$38(a5)
	beq.s	loc_00346A
	neg.w	d5
	neg.w	d7
	exg.l	d5,d7
loc_00346A:
	add.w	d1,d4
	add.w	d1,d6
	add.w	d2,d5
	add.w	d2,d7
	bsr.w	Hitbox_FindPlayerBoxOfId
	bne.s	loc_003480
	lea.l	$2(a0),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_003480:
	movea.l	a0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_003488:
	adda.w	#$A,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetPlayerWindowFromArgs - a2 op: sets the player-relative window of Op2_HoldPlayerWindow from the subroutine arguments g_VM2Arg0..3 (x near, x far, y near, y far)
; In:  a2 = script (no operand); g_VM2Arg0/1/2/3 high words (from Op2_GosubArgs)
; Out: $14/$16(a3) packed by Mot_StorePlayerWindow; NEXT
; Evidence: move.w the four args into d0-d3 / bra Mot_StorePlayerWindow; $34A2 (never run) takes the four words inline instead; 51 runs
; ---------------------------------------------------------------------------
Op2_SetPlayerWindowFromArgs:
	move.w	(g_VM2Arg0).w,d0
	move.w	(g_VM2Arg1).w,d1
	move.w	(g_VM2Arg2).w,d2
	move.w	(g_VM2Arg3).w,d3
	bra.s	Mot_StorePlayerWindow
