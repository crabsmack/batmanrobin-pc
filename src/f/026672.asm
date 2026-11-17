; ---------------------------------------------------------------------------
; Enemy_LadderThug_PlaceOnHull - native fragment after the hull-hung ladder moves: saves its x/y to script vars 1/2, places it at that hull point tilted with the floor (Boss_Stage2_PlaceTurret) and $50 px lower
; In:  a2 script context
; Out: vars 1/2 = moved position, $3E/$42(a6) = screen position; jsr $7FE.w
; Evidence: bsr Boss_Stage2_PlaceTurret, addi.w #$50,$42(a6); TAS f33179-f35225 (1212 frames)
; ---------------------------------------------------------------------------
Enemy_LadderThug_PlaceOnHull:
	move.w	$3E(a6),$24(a4)
	move.w	$42(a6),$26(a4)
	bsr.w	Boss_Stage2_PlaceTurret
	addi.w	#$50,$42(a6)
	jsr	($7FE).w
