; ---------------------------------------------------------------------------
; Boss_Stage2_FxAtHullPoint - boss native: runs the a4 effect script given by the dc.l after two inline words (a point of the hull picture) at that point tilted with the arena floor (Boss_Stage2_TiltPoint)
; In:  a2 = inline x, y words then a dc.l a4 script
; Out: a2 past the dc.l; d2/d3 = 0, d4/d5 = 16.16 position (Fx_RunScriptArg)
; Evidence: op $2CB2 at $025DE0 ($100,$5A, Scr4_025EE0), $025E30 and $025E3E (Scr4_025FD6/$025FDA) in the defeat script; TAS f34683-f35412 (142 frames)
; ---------------------------------------------------------------------------
Boss_Stage2_FxAtHullPoint:
	move.w	(a2)+,d0
	move.w	(a2)+,d1
	bsr.w	Boss_Stage2_TiltPoint
	moveq	#$0,d2
	moveq	#$0,d3
	swap	d4
	swap	d5
	jmp	(Fx_RunScriptArg).l
