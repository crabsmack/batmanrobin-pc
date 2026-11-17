; ---------------------------------------------------------------------------
; Layer_MoveX - moves a layer's x by d0 pixels without drawing, stepping its block column +$26 (wrapping at the map width) by the block boundaries crossed
; In:  a6 = layer, d0 = signed pixels
; Out: (a6) x and +$26 updated
; Evidence: called by the scene-script ops $7396/$73D6 and the vertical stage setup $032F14 (d0 = -$10) before Layer_DrawScreen; ran in the TAS at 21156 and 67239
; ---------------------------------------------------------------------------
Layer_MoveX:
	move.w	(a6),d1
	add.w	d0,d1
	move.w	d1,(a6)
	move.w	d0,d2
	bpl.s	loc_0128C2
	neg.w	d2
	lsr.w	#$4,d2
	neg.w	d2
	neg.w	d0
	andi.w	#$F,d0
	not.w	d1
	andi.w	#$F,d1
	sub.w	d0,d1
	bcc.w	loc_0128D2
	subq.w	#$1,d2
	bra.s	loc_0128D2
loc_0128C2:
	lsr.w	#$4,d2
	andi.w	#$F,d0
	andi.w	#$F,d1
	sub.w	d0,d1
	bcc.s	loc_0128D2
	addq.w	#$1,d2
loc_0128D2:
	move.w	$2E(a6),d1
	add.w	$26(a6),d2
	bpl.s	loc_0128DE
	add.w	d1,d2
loc_0128DE:
	cmp.w	d1,d2
	blt.s	loc_0128E4
	sub.w	d1,d2
loc_0128E4:
	move.w	d2,$26(a6)
	rts
; ---------------------------------------------------------------------------
; Layer_MoveYWithCamera - adds d0 to g_CameraY, then moves the layer's y by d0 (Layer_MoveY)
; In:  a6 = layer, d0 = signed pixels
; Out: g_CameraY, layer +2 and +$28 updated
; Evidence: pointed to from the scene setups $01FF0C and $048AF2; executed in 10 runs (TAS, idle, options, level-skip)
; ---------------------------------------------------------------------------
Layer_MoveYWithCamera:
	add.w	d0,(g_CameraY).l
