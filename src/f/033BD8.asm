; ---------------------------------------------------------------------------
; Stage2_LiftLampInitY - native at the start of the lift lamp's script: converts its trigger y to the scrolling map's screen space (whole pixels - g_MapScrollY + g_LiftVelY + 14 px: addi.l #$E0000), then continues the script (Scr2_033BF4: cruise, Op2_SubLiftVelY rides the lift)
; In:  a6 = the lamp object
; Out: $42(a6) = screen-space y; re-enters the a2 VM ($7FE)
; Evidence: a2 native word $33BD8 in Scr2_033BB8 (op $33BD6); TAS f22282-f31762, 22 spawns
; ---------------------------------------------------------------------------
Stage2_LiftLampInitY:
	move.l	$42(a6),d0
	clr.w	d0
	sub.l	(g_MapScrollY).w,d0
	add.l	(g_LiftVelY).w,d0
	addi.l	#$E0000,d0
	move.l	d0,$42(a6)
	jsr	($7FE).w
