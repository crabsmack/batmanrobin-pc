; ---------------------------------------------------------------------------
; Enemy_BlackDrone_NextMuzzle - native switch in the black gun drone's animation (flying section, body $027DBE): when script var 0 (shot requests) changed since the last call (var 1), stores it and returns 1 / 2 by its parity to fire from the upper ($10,-$B) or lower ($10,$B) muzzle, else 0
; In:  a4 +$22 var 0, +$24 var 1
; Out: d0 = 0 / 1 / 2 for Op2_CallNativeRelSwitch
; Evidence: op $2D30 at $027E74 ... $027F4E in the body's anim; var 0 bumped by Op2_IncVar0 at $027E3A after Op2_BranchTouchBox; RAM f35460: such an object entering at x 297; 'black gun drone' = the flight-enemy identification in NOTES (body $27DBE, energy $68); TAS f35434-f57184 (1552)
; ---------------------------------------------------------------------------
Enemy_BlackDrone_NextMuzzle:
	move.w	$22(a4),d0
	cmp.w	$24(a4),d0
	beq.s	loc_027E54
	move.w	d0,$24(a4)
	andi.w	#$1,d0
	addq.w	#$1,d0
	rts
loc_027E54:
	moveq	#$0,d0
	rts
