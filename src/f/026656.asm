; ---------------------------------------------------------------------------
; Enemy_LadderThug_LoadHullPos - native fragment at the top of the hull-hung ladder's frame: restores x/y from script vars 1/2 (its untilted hull-space position) before the motion ops
; In:  a2 script context, a4 +$24/$26
; Out: $3E/$42(a6) = vars 1/2; jsr $7FE.w
; Evidence: pairs with Enemy_LadderThug_PlaceOnHull in the loop $026654-$026694; TAS f33180-f35226 (1212 frames)
; ---------------------------------------------------------------------------
Enemy_LadderThug_LoadHullPos:
	move.w	$24(a4),$3E(a6)
	move.w	$26(a4),$42(a6)
	jsr	($7FE).w
