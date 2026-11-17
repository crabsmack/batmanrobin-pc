; ---------------------------------------------------------------------------
; Enemy_BombRabbit_BlastRecent - switch native of a lit bomb rabbit's fuse: d0 = 1 while g_ScriptTimer runs (another rabbit exploded less than 20 frames ago: keep ticking), else 0 (may explode when its fuse timer 2 runs out)
; In:  -
; Out: d0 = 0 or 1 (Op2_CallNativeRelSwitch case)
; Evidence: `2D30 0024 0004 0006` at $35E72 (fuse coroutine started at $35E68: timer 2 = $3C0); g_ScriptTimer set by Enemy_BombRabbit_SetBlastGap; TAS f82646-f115353 (247 frames)
; ---------------------------------------------------------------------------
Enemy_BombRabbit_BlastRecent:
	move.w	(g_ScriptTimer).w,d0
	ble.s	loc_035EA0
	moveq	#$1,d0
loc_035EA0:
	rts
