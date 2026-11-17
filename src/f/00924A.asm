; ---------------------------------------------------------------------------
; Arc_HeadAtMark - a4 native: points the object's heading at the marked point (g_ArcTargetX/Y, set by Op2_FxMarkPlayerTarget): heading = Mot_Atan2 of the mark minus the object's x/y
; In:  a6 = the a4 object (+$2E x, +$32 y)
; Out: $2A(a6) = the heading (0-$3FF); d0/d1 = the offsets, d2 = the heading (Mot_Atan2's)
; Evidence: Op4_CallNative's dc.l at $0298DA and $02996E (stage 2 a4 spawn scripts after Op2_FxMarkPlayerTarget); move.w g_ArcTargetX,d0 / move.w g_ArcTargetY,d1 / sub.w $2E(a6),d0 / sub.w $32(a6),d1 / jsr Mot_Atan2 / move.w d2,$2A(a6); no corpus run reaches it: raw until crane-crash's scan of script call operands (2026-10-30)
; ---------------------------------------------------------------------------
Arc_HeadAtMark:
	move.w	(g_ArcTargetX).w,d0
	move.w	(g_ArcTargetY).w,d1
	sub.w	$2E(a6),d0
	sub.w	$32(a6),d1
	jsr	(Mot_Atan2).l
	move.w	d2,$2A(a6)
	rts
