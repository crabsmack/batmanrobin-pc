; ---------------------------------------------------------------------------
; Boss_Hatter_RoadHazardInit - a2 native fragment (op $07FC at $03DE16) that starts the road hazard's body: sets one sprite frame slot (+$14(a5) = 1), then the body script Scr2_03DE22 follows
; In:  a5 = the hazard's sprite block
; Out: +$14(a5)
; Evidence: Op2_Goto from Scr2_03FB1E to $03DE16; body: Op2_SetSpeed, then a loop of Op2_CallNativeRelSwitch $03DE80 (0 draw, 1 attack box + Op2_TestAttackBoxAtDrawn, 2 Op2_Kill); TAS f114753/f114833
; ---------------------------------------------------------------------------
Boss_Hatter_RoadHazardInit:
	move.w	#$1,$14(a5)
	jsr	($7FE).w
