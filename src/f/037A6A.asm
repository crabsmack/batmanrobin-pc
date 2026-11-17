; ---------------------------------------------------------------------------
; ToyChain_SteerAtPlayer - switch native of a chain's steering coroutine: after a lost link (var 0 bit 4) speeds the sway up by 1/4; sways toward the targeted player's x while the player is above the tip; when the tip drops below the base it mirrors the chain (heading + $200, sway negated); reports whether base and tip are off screen
; In:  a6 = chain object (a3 motion: +$20 accel, +$24 target speed, +$30 cruise), a4 +$10 targeted player record, $E(a6) chain block (tip +$30/+$32)
; Out: d0 = 0 on screen, 1 mirrored, 2 both ends off screen (-100..$1A4 from g_CameraX)
; Evidence: eori #$10,$22(a4); asr.l #2 accel/cruise; record +$2E/+$32 vs tip; eori.w #$200,$2E(a0) / neg.l $3E(a6); Op2_CallNativeRelSwitch $037A32 in coroutine 2 of the chain body (case 1: dust puffs $10D9A/$10DAC and sound $B; case 2: ToyChain_RemoveAllLinks and Op2_Kill); TAS f83917-f110830 (1319 frames)
; ---------------------------------------------------------------------------
ToyChain_SteerAtPlayer:
	move.w	$22(a4),d0
	andi.w	#$10,d0
	beq.s	loc_037AA0
	eori.w	#$10,$22(a4)
	move.l	$20(a3),d0
	asr.l	#$2,d0
	add.l	d0,$20(a3)
	move.l	$30(a3),d0
	asr.l	#$2,d0
	add.l	$30(a3),d0
	move.l	d0,$30(a3)
	tst.l	$24(a3)
	beq.s	loc_037AA0
	bpl.s	loc_037A9C
	neg.l	d0
loc_037A9C:
	move.l	d0,$24(a3)
loc_037AA0:
	movea.w	$E(a6),a0
	move.w	$10(a4),d0
	beq.s	loc_037ACA
	movea.w	d0,a1
	move.w	$32(a1),d0
	cmp.w	$32(a0),d0
	bgt.s	loc_037ACA
	move.l	$30(a3),d1
	move.w	$2E(a1),d0
	cmp.w	$30(a0),d0
	bgt.s	loc_037AC6
	neg.l	d1
loc_037AC6:
	move.l	d1,$24(a3)
loc_037ACA:
	move.w	$32(a0),d0
	cmp.w	$50(a6),d0
	ble.s	loc_037AE8
	eori.w	#$200,$2E(a0)
	neg.l	$3E(a6)
	move.w	$30(a0),$4E(a6)
	moveq	#$1,d0
	rts
loc_037AE8:
	move.w	$4E(a6),d0
	move.w	$30(a0),d1
	cmp.w	d1,d0
	bge.s	loc_037AF6
	exg.l	d0,d1
loc_037AF6:
	sub.w	(g_CameraX).w,d0
	sub.w	(g_CameraX).w,d1
	cmpi.w	#$FF9C,d0
	blt.s	loc_037B0E
	cmpi.w	#$1A4,d1
	bgt.s	loc_037B0E
	moveq	#$0,d0
	rts
loc_037B0E:
	moveq	#$2,d0
	rts
