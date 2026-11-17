; ---------------------------------------------------------------------------
; Shake_Start - starts the standard screen shake: 6 px up, pull 2 px per frame, and kicks the second oscillator
; In:  none
; Out: g_ShakeVel/g_ShakeGravity/g_ShakeY, g_Shake2Pull
; Evidence: callers a4 op $64A8, $240EE and 25 script uses ($137A0...); Shake_Update bounces it down to rest
; ---------------------------------------------------------------------------
Shake_Start:
	move.l	#$60000,(g_ShakeVel).w
	move.l	#$20000,(g_ShakeGravity).w
; ---------------------------------------------------------------------------
; Shake_Arm - shared tail of the Shake_Start* entries: g_ShakeY = 0, g_Shake2Pull = $2000, g_Shake2Vel = 4.0 when g_Shake2Pos <= 12
; In:  g_ShakeVel/g_ShakeGravity set by the caller
; Out: g_ShakeY = 0 (the bounce starts from rest)
; Evidence: branched to by Shake_Start, Shake_StartLongIfIdle, Shake_StartBig, Shake_StartRandomIfIdle
; ---------------------------------------------------------------------------
Shake_Arm:
	clr.l	(g_ShakeY).w
	move.l	#$2000,(g_Shake2Pull).w
	cmpi.w	#$C,(g_Shake2Pos).w
	bgt.s	Shake_Done
	move.w	#$4,(g_Shake2Vel).w
; ---------------------------------------------------------------------------
; Shake_Done - the rts the shake starters share (a shake already running, or the second oscillator left alone)
; In:  none
; Out: none (the condition codes of the test before)
; Evidence: bne.s from Shake_StartIfIdle, Shake_StartLongIfIdle, Shake_StartBigIfIdle, Shake_StartRandomIfIdle; bgt.s in Shake_Arm
; ---------------------------------------------------------------------------
Shake_Done:
	rts
; ---------------------------------------------------------------------------
; Shake_StartLongIfIdle - native callback: starts a 6 px shake with a weak pull (1.5), unless a shake is running
; In:  none
; Out: g_ShakeVel = 6.0, g_ShakeGravity = 1.5 when started
; Evidence: script use $488F2
; ---------------------------------------------------------------------------
Shake_StartLongIfIdle:
	tst.l	(g_ShakeY).w
	bne.s	Shake_Done
; ---------------------------------------------------------------------------
; Shake_StartLong - native callback: starts a 6 px shake with the weak pull (1.5) unconditionally, even over a running one
; In:  none
; Out: g_ShakeVel = 6.0, g_ShakeGravity = 1.5, then Shake_Arm's outputs
; Evidence: Shake_StartLongIfIdle's body after its idle test; a runtime entry of the reference (a script's native-call op names $95D4 itself: P4 entries)
; ---------------------------------------------------------------------------
Shake_StartLong:
	move.l	#$60000,(g_ShakeVel).w
	move.l	#$18000,(g_ShakeGravity).w
	bra.s	Shake_Arm
; ---------------------------------------------------------------------------
; Shake_StartBigIfIdle - native callback: starts the big shake unless one is running
; In:  none
; Out: g_ShakeVel = 12.0, g_ShakeGravity = 3.0 when started
; Evidence: returns when g_ShakeY is nonzero, else Shake_StartBig; script uses $48810, $48828
; ---------------------------------------------------------------------------
Shake_StartBigIfIdle:
	tst.l	(g_ShakeY).w
	bne.s	Shake_Done
