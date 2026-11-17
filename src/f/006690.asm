; ---------------------------------------------------------------------------
; Op4_TakeHitPhaseA - a4 op: gives the object the next frame phase $18 from the toggling g_FxPhaseA, so successive player shots run their hit tests on alternate frames
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: move.b $F4B2,$18(a6) / not.b $F4B2; 36 uses, all in the player shot scripts Scr4_05034A (e.g. $5051E `6690 695E 0E00`); 151 corpus runs
; ---------------------------------------------------------------------------
Op4_TakeHitPhaseA:
	move.b	(g_FxPhaseA).w,$18(a6)
	not.b	(g_FxPhaseA).w
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_TakeHitPhaseB - a4 op: as Op4_TakeHitPhaseA from the second toggle g_FxPhaseB (enemy bullets)
; In:  a4 = script (no operands)
; Out: NEXT
; Evidence: move.b $F4B3,$18(a6) / not.b $F4B3; script uses $42966, $42F52, $46B52, each before Op4_HitPlayersOnPhase; 10 corpus runs
; ---------------------------------------------------------------------------
Op4_TakeHitPhaseB:
	move.b	(g_FxPhaseB).w,$18(a6)
	not.b	(g_FxPhaseB).w
	movea.w	(a4)+,a0
	jmp	(a0)
