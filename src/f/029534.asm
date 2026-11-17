; ---------------------------------------------------------------------------
; Boss_BeamShip_FireAtSpot - ship native: fires an a4 shot script from the ship's nose (x - $18) at the heading from the ship to the beam spot ($46/$4A) plus a word offset
; In:  a2 = script: word heading offset, dc.l a4 script; a6 = the ship
; Out: a2 past the arguments (Fx_RunScriptArg)
; Evidence: op $2CB2 in the attack scripts $2E2A8/$2E5A6 in fans of 3-6 (offsets -$3C, 0, $3C ..., script $2E9CE); fuzz-77400-s2 only
; ---------------------------------------------------------------------------
Boss_BeamShip_FireAtSpot:
	move.w	$46(a6),d0
	sub.w	$3E(a6),d0
	move.w	$4A(a6),d1
	sub.w	$42(a6),d1
	jsr	(Mot_Atan2).l
	move.w	d2,d3
	add.w	(a2)+,d3
	swap	d3
	moveq	#$0,d2
	move.l	$3E(a6),d4
	subi.l	#$180000,d4
	move.l	$42(a6),d5
	jmp	(Fx_RunScriptArg).l
; ---------------------------------------------------------------------------
; Boss_BeamShip_SetShipVertex - ship native, every frame: stores the ship's x/y as span vertex 0 (g_TriX0/Y0, the beam's source; the callback subtracts $18), then falls into Boss_BeamShip_RunRadiusObj
; In:  a6/a4 = the ship
; Out: g_TriX0/g_TriY0; then as Boss_BeamShip_RunRadiusObj
; Evidence: op $2CB2 at $29310 and $2936E in the ship's main and death loops (Scr2_02928A); TAS 77185-79028
; ---------------------------------------------------------------------------
Boss_BeamShip_SetShipVertex:
	move.w	$3E(a6),(g_TriX0).w
	move.w	$42(a6),(g_TriY0).w
