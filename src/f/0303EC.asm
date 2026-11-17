; ---------------------------------------------------------------------------
; Enemy_PipeCannon_PipePhase - animation switch native of the pipe cannon: d0 = bit 1 of its map y ($50), which picks one of two frames whose pipe stripes are offset, so the pipe seems to slide past while the cannon moves
; In:  a6 = the pipe cannon (map position in $4E/$50)
; Out: d0 = 0 or 1 (case of Op2_CallNativeRelSwitch)
; Evidence: op $2D30 at $304BE (intact frames $415A/$4164) and $304F4 (broken frames $416E/$4178) in the anim scripts of Scr2_030416; frames rendered: a cannon block on a vertical pipe; TAS 69665-74542 (348 frames)
; ---------------------------------------------------------------------------
Enemy_PipeCannon_PipePhase:
	move.w	$50(a6),d0
	lsr.w	#$1,d0
	andi.w	#$1,d0
	rts
; ---------------------------------------------------------------------------
; Flight_AirshipLoadMapPos - loads a map-anchored object's map position ($4E/$50) into its x/y ($3E/$42), so the following move ops integrate its motion in airship map space
; In:  a6 = object
; Out: $3E/$42 = $4E/$50
; Evidence: op $2CB2 at $30432 and $30472 in the pipe cannon's loop (then Op2_StepVelocity, Op2_ApplyMove, Flight_AirshipStoreMapPos); TAS 69585-74542 (pipe cannons on screen)
; ---------------------------------------------------------------------------
Flight_AirshipLoadMapPos:
	move.w	$4E(a6),$3E(a6)
	move.w	$50(a6),$42(a6)
	rts
; ---------------------------------------------------------------------------
; Flight_AirshipStoreMapPos - stores a moved object's x/y as its map position ($4E/$50) and converts back to world x/y with Flight_AirshipMapToWorld
; In:  a6 = object, $3E/$42 = its new map-space position
; Out: $4E/$50 set, $3E/$42 = world position
; Evidence: op $2CB2 at $3047C in the pipe cannon's loop, right after Op2_ApplyMove; pairs with Flight_AirshipLoadMapPos; TAS 69585-74542
; ---------------------------------------------------------------------------
Flight_AirshipStoreMapPos:
	move.w	$3E(a6),$4E(a6)
	move.w	$42(a6),$50(a6)
	bra.w	Flight_AirshipMapToWorld
