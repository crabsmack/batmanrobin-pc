; ---------------------------------------------------------------------------
; Enemy_Biplane_RandAnimPhase - red biplane native: starts its propeller animation counter (sprite var 0, $22(a5)) at a random value so planes in a wave don't flap in step
; In:  a5 = the plane's sprite block
; Out: $22(a5) = Rand_Next
; Evidence: op $2CB2 (CallNativeRel) at $282B0 at the start of the biplane body $28286 (anim Scr2_028326, frames $38CC-$393A rendered: a small red propeller plane); TAS 36400-66213 (108 frames, one per plane spawned)
; ---------------------------------------------------------------------------
Enemy_Biplane_RandAnimPhase:
	jsr	(Rand_Next).l
	move.w	d0,$22(a5)
	rts
; ---------------------------------------------------------------------------
; Enemy_Biplane_NextPropFrame - red biplane anim native (switch): steps sprite var 0 and returns which of 4 propeller frames to draw: bit 0 alternates the two blur frames, +2 (the bright-hub frames) on 4 of every 32 steps
; In:  a5 = the plane's sprite block
; Out: d0 = 0-3 (case of the switch); $22(a5) incremented
; Evidence: op $2D30 (CallNativeRelSwitch) at $2832E/$283B6 in the anim script Scr2_028326; cases draw frames $38CC/$38D6 (blur) and $38E0/$38EA (bright hub)
; ---------------------------------------------------------------------------
Enemy_Biplane_NextPropFrame:
	addq.w	#$1,$22(a5)
	move.w	$22(a5),d0
	move.w	d0,d1
	andi.w	#$1,d0
	andi.w	#$1C,d1
	bne.s	loc_028324
	addq.w	#$2,d0
loc_028324:
	rts
