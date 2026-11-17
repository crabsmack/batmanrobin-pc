; ---------------------------------------------------------------------------
; Boss_Hatter_StepBounce - a2 native fragment (op $07FC at $03D262, coroutine 0) run every frame after the defeat: the drop offset falls under gravity (speed += 1/16 px per frame) and bounces off 64 px with 3/4 of its speed; the hat is drawn that far below its position ($4E/$50 = x, y + offset)
; In:  a4 +$22.l speed, +$26.l offset (16.16), a6 = the boss
; Out: a4 +$22/+$26, drawn position $4E/$50(a6)
; Evidence: addi.l #$1000 / cmpi.l #$400000 / speed - speed/4 negated; started by Op2_StartCoroutine0Here at $03D260; TAS f115654-f116145 (492 frames); png f115800: the defeated hat lower on the screen
; ---------------------------------------------------------------------------
Boss_Hatter_StepBounce:
	move.l	$22(a4),d0
	move.l	$26(a4),d1
	addi.l	#$1000,d0
	add.l	d0,d1
	cmpi.l	#$400000,d1
	ble.s	loc_03D28A
	move.l	d0,d1
	asr.l	#$2,d1
	sub.l	d1,d0
	neg.l	d0
	move.l	#$400000,d1
loc_03D28A:
	move.l	d0,$22(a4)
	move.l	d1,$26(a4)
	swap	d1
	add.w	$42(a6),d1
	move.w	d1,$50(a6)
	move.w	$3E(a6),$4E(a6)
	jsr	($7FE).w
