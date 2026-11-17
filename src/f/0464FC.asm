; ---------------------------------------------------------------------------
; Boss_Freeze_DropStep - one frame of Mr. Freeze's drop off his broken hover platform: adds the frame's y move to the drop offset (sub-block +$C, 16.16), clamps it at $50 px and draws him at the platform's x, y + offset
; In:  a6 = boss, a3 = its sub-block $12(a6), d2 = this frame's y move (16.16, from Op2_StepVelocity)
; Out: d0 = 0 still falling, 1 landed ($50 px below); drawn x/y $4E/$50(a6) set
; Evidence: only caller: Op2_CallNativeRelSwitch at $046434 in Scr2_046176, after the 15% energy break (sound $11, Shake_Start, Fade_FlashWhite3, explosion, Op2_JumpAnim on sub-block $12); case 1 gives him one body box and a new attack coroutine; TAS 130401-130448 (f130380 on the platform, f130460 on the floor below it)
; ---------------------------------------------------------------------------
Boss_Freeze_DropStep:
	add.l	$C(a3),d2
	move.l	d2,$C(a3)
	swap	d2
	moveq	#$0,d0
	cmpi.w	#$50,d2
	blt.s	loc_046514
	moveq	#$1,d0
	move.w	#$50,d2
loc_046514:
	add.w	$42(a6),d2
	move.w	d2,$50(a6)
	move.w	$3E(a6),$4E(a6)
	rts
