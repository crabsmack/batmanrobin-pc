; ---------------------------------------------------------------------------
; Op2_SwitchHeading - a2 op: branches by the current heading split into n equal sectors: index = ((heading $C(a3) + rotation) & $3FE) * n / 1024
; In:  a2 = script: word rotation, word n, then n word offsets
; Out: NEXT at the chosen case
; Evidence: add.w (a2)+ / andi #$3FE / mulu.w (a2)+ / lsl.l #6 / swap / adda; 45 corpus runs; $2FA14 `3F6C 0120 0010` (16 sectors)
; ---------------------------------------------------------------------------
Op2_SwitchHeading:
	move.w	$C(a3),d0
	add.w	(a2)+,d0
	andi.w	#$3FE,d0
	mulu.w	(a2)+,d0
	lsl.l	#$6,d0
	swap	d0
	add.w	d0,d0
	adda.w	d0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SwitchHeadingClamp - a2 op: as Op2_SwitchHeading but the sector index minus a base is clamped to [0, max] before indexing the table
; In:  a2 = script: word rotation, word n, word base, word max, then max+1 word offsets
; Out: NEXT at the chosen case
; Evidence: sub.w (a2)+,d0 / bpl / cmp.w (a2)+,d0 / ble / move.w -2(a2),d0; 26 corpus runs; $26322 `3F86 010C 0028 000F 000A`
; ---------------------------------------------------------------------------
Op2_SwitchHeadingClamp:
	move.w	$C(a3),d0
	add.w	(a2)+,d0
	andi.w	#$3FE,d0
	mulu.w	(a2)+,d0
	lsl.l	#$6,d0
	swap	d0
	sub.w	(a2)+,d0
	bpl.s	loc_003F9C
	moveq	#$0,d0
loc_003F9C:
	cmp.w	(a2)+,d0
	ble.s	loc_003FA4
	move.w	-$2(a2),d0
loc_003FA4:
	add.w	d0,d0
	adda.w	d0,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
