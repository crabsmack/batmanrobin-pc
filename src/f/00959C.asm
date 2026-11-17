; ---------------------------------------------------------------------------
; Shake_StartIfIdle - native callback: starts the standard screen shake unless one is running
; In:  none
; Out: g_ShakeVel = 6.0, g_ShakeGravity = 2.0 when started
; Evidence: returns when g_ShakeY is nonzero, else falls into Shake_Start; script use $1C5E8 (TAS f19339-f20434)
; ---------------------------------------------------------------------------
Shake_StartIfIdle:
	tst.l	(g_ShakeY).w
	bne.s	Shake_Done
