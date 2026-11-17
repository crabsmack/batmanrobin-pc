; ---------------------------------------------------------------------------
; Shake_StartBig - starts the big screen shake: 12 px up, pull 3
; In:  none
; Out: g_ShakeVel = 12.0, g_ShakeGravity = 3.0
; Evidence: callers $2417E and 16 script uses ($20D54, $26B52, $3C774...)
; ---------------------------------------------------------------------------
Shake_StartBig:
	move.l	#$C0000,(g_ShakeVel).w
	move.l	#$30000,(g_ShakeGravity).w
	bra.s	Shake_Arm
; ---------------------------------------------------------------------------
; Shake_StartRandomIfIdle - native callback: unless a shake runs, starts one 3-5 px up (random), pull 2
; In:  none
; Out: g_ShakeVel = 3.0-5.0, g_ShakeGravity = 2.0 when started
; Evidence: Rand_Below(3) + 3; script uses $3B6A8, $3B860, $3CC52
; ---------------------------------------------------------------------------
Shake_StartRandomIfIdle:
	tst.l	(g_ShakeY).w
	bne.s	Shake_Done
; ---------------------------------------------------------------------------
; Shake_StartRandom - native callback: starts a 3-5 px shake (random), pull 2, unconditionally
; In:  none
; Out: g_ShakeVel = 3.0-5.0, g_ShakeGravity = 2.0, then Shake_Arm's outputs; d0 = the speed
; Evidence: Shake_StartRandomIfIdle's body after its idle test; a runtime entry of the reference (a script's native-call op names $9604 itself: P4 entries)
; ---------------------------------------------------------------------------
Shake_StartRandom:
	move.w	#$3,d0
	bsr.w	Rand_Below
	addq.w	#$3,d0
	swap	d0
	move.l	d0,(g_ShakeVel).w
	move.l	#$20000,(g_ShakeGravity).w
	bra.s	Shake_Arm
; ---------------------------------------------------------------------------
; Shake_Rumble - every 4th frame: kicks the shake 3-7 px up (random, pull 2) and steps it
; In:  none
; Out: on kick frames g_ShakeVel = 3.0-7.0, g_ShakeGravity = 2.0, then Shake_Update's outputs
; Evidence: frame counter low byte ($E031) & 3; Rand_Below(5) + 3, then falls into Shake_Update; caller $2417E
; ---------------------------------------------------------------------------
Shake_Rumble:
	move.b	($FFFFE031).w,d0
	andi.b	#$3,d0
	beq.s	loc_00962A
	rts
loc_00962A:
	move.w	#$5,d0
	bsr.w	Rand_Below
	addq.w	#$3,d0
	swap	d0
	move.l	d0,(g_ShakeVel).w
	move.l	#$20000,(g_ShakeGravity).w
