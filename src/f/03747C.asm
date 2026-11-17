; ---------------------------------------------------------------------------
; ToyChain_TakeLinkHits - switch native: collects hits on the chain's links: clears each link's struck bit, subtracts its damage from the chain energy $52(a6) and remembers it in g_StruckBox; when the energy goes negative it is reset to one link's energy (the script then removes a link)
; In:  a6 = chain object, $E(a6) = chain block (+$20 energy per link), a4 +$24 = link count
; Out: d0 = 0 not hit, 1 hit, 2 a link's worth of energy used up
; Evidence: bclr #0,$3C(a0) / sub.w $20(a0),$52(a6) / move.l a0,g_StruckBox; Op2_CallNativeRelSwitch at $0379EE (case 2 -> ToyChain_RemoveLastLink, award, explosion), $037BA8, $037DE8, $038040; TAS 6492 frames
; ---------------------------------------------------------------------------
ToyChain_TakeLinkHits:
	moveq	#$0,d0
	move.w	$24(a4),d3
	subq.w	#$1,d3
	bmi.s	loc_0374C0
	movea.w	$E(a6),a1
	lea.l	$34(a1),a1
loc_03748E:
	movea.w	(a1)+,a0
	bclr	#$0,$3C(a0)
	beq.s	loc_0374AA
	moveq	#$1,d0
	move.w	$20(a0),d1
	clr.w	$20(a0)
	sub.w	d1,$52(a6)
	move.l	a0,(g_StruckBox).w
loc_0374AA:
	dbra	d3,loc_03748E
	tst.w	$52(a6)
	bpl.s	loc_0374C0
	movea.w	$E(a6),a1
	move.w	$20(a1),$52(a6)
	moveq	#$2,d0
loc_0374C0:
	rts
