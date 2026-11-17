; ---------------------------------------------------------------------------
; ToyChain_AddLink - adds one link to a chain: a pool block used as a collision box (no-op hit callback $82C, owner a6, id $58(a6), serial $56(a6), the chain's link hit callback and damage/knockback args), stored at +$34 + 2*count of the chain block
; In:  a2 = chain block, a4 +$24 = link count, a6 = chain object
; Out: d0 = 0 and count + 1, or nonzero when the pool is empty (Z clear)
; Evidence: ObjPool_Alloc; $30 = $82C, $2E = a6, $10 = $58(a6), $3E = $26(a2), $34/$36 = $22/$24(a2); boxes are linked by ToyChain_PlaceLinkBoxes; TAS 561 frames
; ---------------------------------------------------------------------------
ToyChain_AddLink:
	move.w	$24(a4),d0
	add.w	d0,d0
	lea.l	$34(a2,d0.w),a1
	jsr	(ObjPool_Alloc).l
	bne.s	loc_0372B6
	move.l	#$82C,$30(a0)
	clr.w	$2(a0)
	clr.w	$3C(a0)
	clr.w	$20(a0)
	move.w	a6,$2E(a0)
	move.w	$58(a6),$10(a0)
	move.w	$56(a6),d0
	move.w	d0,$4(a0)
	clr.w	$42(a0)
	move.l	$26(a2),$3E(a0)
	move.w	$22(a2),$34(a0)
	move.w	$24(a2),$36(a0)
	move.w	a0,(a1)+
	addq.w	#$1,$24(a4)
	moveq	#$0,d0
loc_0372B6:
	rts
