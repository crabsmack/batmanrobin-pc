; ---------------------------------------------------------------------------
; Mot_StorePlayerWindow - packs a player-relative window into the motion block: halves of d0/d1 into the bytes of $14(a3), of d2/d3 into $16(a3) (Op2_HoldPlayerWindow doubles them back), then continues the script
; In:  d0/d1 = x window lo/hi, d2/d3 = y window lo/hi (pixels, signed, even)
; Out: $14.w(a3) = d0/2 : d1/2, $16.w(a3) = d2/2 : d3/2; NEXT
; Evidence: asl.w #7 / asr.w #1 / move.b; $34BE reads the bytes back with ext.w and add.w (x2); shares $14/$16(a3) with the target point; 51 runs
; ---------------------------------------------------------------------------
Mot_StorePlayerWindow:
	asl.w	#$7,d0
	asr.w	#$1,d1
	move.b	d1,d0
	asl.w	#$7,d2
	asr.w	#$1,d3
	move.b	d3,d2
	move.w	d0,$14(a3)
	move.w	d2,$16(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_HoldPlayerWindow - a2 op: steers both axes to keep the object inside a window around the targeted player (x lo..hi, y lo..hi from $14/$16(a3), + jitter $1C(a6)), first pulling it back on screen (screen x $1E-$122, y $1E-$C2); no target brakes both axes
; In:  a2 = script (no operand); window from Op2_SetPlayerWindowFromArgs
; Out: x/y target speeds $24/$2C(a3) = +-cruise or 0; NEXT
; Evidence: move.b $14(a3)/$15(a3) ext.w add.w + $2E(a0) + $1C(a6); cmpi.w #$122/#$1E/#$C2 against g_CameraX/Y-relative positions; 53 runs; $37EF2 follows `2E4E 000E` with `34BE`
; ---------------------------------------------------------------------------
Op2_HoldPlayerWindow:
	move.w	$10(a4),d0
	beq.w	Op2_ClearTargetSpeeds
	movea.w	d0,a0
	move.w	$3E(a6),d1
	move.w	d1,d2
	sub.w	(g_CameraX).w,d2
	cmpi.w	#$122,d2
	bgt.s	loc_003502
	cmpi.w	#$1E,d2
	blt.s	loc_00350E
	move.b	$14(a3),d3
	move.b	$15(a3),d4
	ext.w	d3
	add.w	d3,d3
	ext.w	d4
	add.w	d4,d4
	move.w	$1C(a6),d0
	add.w	$2E(a0),d0
	add.w	d0,d3
	add.w	d0,d4
	cmp.w	d3,d1
	blt.s	loc_00350E
	cmp.w	d4,d1
	blt.s	loc_003516
loc_003502:
	move.l	$30(a3),$24(a3)
	neg.l	$24(a3)
	bra.s	loc_00351A
loc_00350E:
	move.l	$30(a3),$24(a3)
	bra.s	loc_00351A
loc_003516:
	clr.l	$24(a3)
loc_00351A:
	move.w	$42(a6),d1
	move.w	d1,d2
	sub.w	(g_CameraY).w,d2
	cmpi.w	#$C2,d2
	bgt.s	loc_00355C
	cmpi.w	#$1E,d2
	blt.s	loc_00356A
	move.b	$16(a3),d3
	move.b	$17(a3),d4
	ext.w	d3
	add.w	d3,d3
	ext.w	d4
	add.w	d4,d4
	move.w	$1C(a6),d0
	add.w	$32(a0),d0
	add.w	d0,d3
	add.w	d0,d4
	cmp.w	d3,d1
	blt.s	loc_00356A
	cmp.w	d4,d1
	bgt.s	loc_00355C
	clr.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00355C:
	move.l	$34(a3),$2C(a3)
	neg.l	$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00356A:
	move.l	$34(a3),$2C(a3)
	movea.w	(a2)+,a0
	jmp	(a0)
