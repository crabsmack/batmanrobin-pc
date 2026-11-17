; ---------------------------------------------------------------------------
; Op2_FxSpawnOffAbs - a2 op: as Op2_FxSpawnOff with an absolute (dc.l) a4 script address
; In:  a2 = script: byte dx, byte dy, long a4 script
; Out: NEXT
; Evidence: byte offsets then $520C; move.l (a2)+,d0 / movea.l d0,a4; 22 corpus runs; $254B2 `51F2 FE08 0001 0D9A`
; ---------------------------------------------------------------------------
Op2_FxSpawnOffAbs:
	move.b	(a2)+,d4
	move.b	(a2)+,d5
	ext.w	d4
	ext.w	d5
	add.w	$3E(a6),d4
	add.w	$42(a6),d5
	bra.s	loc_00520C
; ---------------------------------------------------------------------------
; Op2_FxSpawnHereAbs - a2 op: as Op2_FxSpawnHere with an absolute (dc.l) a4 script address
; In:  a2 = script: long a4 script
; Out: NEXT
; Evidence: move.w $3E/$42(a6) then the dc.l spawn tail; 80 corpus runs; $1E078 `5204 0001 4C7E`
; ---------------------------------------------------------------------------
Op2_FxSpawnHereAbs:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
loc_00520C:
	moveq	#$0,d2
	moveq	#$0,d3
; ---------------------------------------------------------------------------
; Op2_FxSpawnRegsAbs - a2 op: as Op2_FxSpawnRegs with an absolute (dc.l) a4 script address
; In:  a2 = script: long a4 script; d2-d5 from the Op2_FxPos*/Heading* ops
; Out: NEXT
; Evidence: dispatched at $5210 (11 corpus runs); $37A42 `5210 0001 0D9A 5284 5294 0200 5210`
; ---------------------------------------------------------------------------
Op2_FxSpawnRegsAbs:
	movem.l	$18(a3),d6-d7
	move.l	(a2)+,d0
	movem.l	a1-a6,-(a7)
	lea.l	(g_SpawnCtx).w,a6
	movem.l	d6-d7,$3E(a6)
	clr.l	$36(a6)
	clr.l	$3A(a6)
	move.w	$38(a5),$22(a6)
	move.w	$3A(a5),$24(a6)
	movea.l	d0,a4
	movea.w	(a4)+,a0
	swap	d4
	swap	d5
	jsr	(a0)
	movem.l	(a7)+,a1-a6
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosScreenCentre - a2 op: sets the effect-spawn registers to the screen centre (g_CameraX + $A0, g_CameraY + $70), radius and heading 0
; In:  a2 = script (no operand)
; Out: d4/d5 = position, d2 = d3 = 0; NEXT
; Evidence: addi.w #$A0,d4 / addi.w #$70,d5; 4 corpus runs; $1E848 `524C 5294 0092 5198`
; ---------------------------------------------------------------------------
Op2_FxPosScreenCentre:
	move.w	(g_CameraX).w,d4
	move.w	(g_CameraY).w,d5
	addi.w	#$A0,d4
	addi.w	#$70,d5
	moveq	#$0,d2
	moveq	#$0,d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosHere - a2 op: sets the effect-spawn registers to the object's position, radius and heading 0
; In:  a2 = script (no operand)
; Out: d4/d5 = $3E/$42(a6), d2 = d3 = 0; NEXT
; Evidence: 109 corpus runs, 154 script uses; $1AFE8 `5264 5172 05E4`
; ---------------------------------------------------------------------------
Op2_FxPosHere:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	moveq	#$0,d2
	moveq	#$0,d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosAnchor - a2 op: sets the effect-spawn registers to the object's anchor (saved) position $46/$4A(a6), radius and heading 0
; In:  a2 = script (no operand)
; Out: d4/d5 = $46/$4A(a6), d2 = d3 = 0; NEXT
; Evidence: 27 corpus runs; $37A02 `5274 5198 F582`
; ---------------------------------------------------------------------------
Op2_FxPosAnchor:
	move.w	$46(a6),d4
	move.w	$4A(a6),d5
	moveq	#$0,d2
	moveq	#$0,d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosDrawn - a2 op: sets the effect-spawn registers to the object's drawn position $4E/$50(a6), radius and heading 0
; In:  a2 = script (no operand)
; Out: d4/d5 = $4E/$50(a6), d2 = d3 = 0; NEXT
; Evidence: 16 corpus runs; $19A30 `5284 5198 0B64`
; ---------------------------------------------------------------------------
Op2_FxPosDrawn:
	move.w	$4E(a6),d4
	move.w	$50(a6),d5
	moveq	#$0,d2
	moveq	#$0,d3
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosAddB - a2 op: adds a signed byte offset to the effect-spawn position d4/d5 (not mirrored)
; In:  a2 = script: byte dx, byte dy
; Out: d4/d5 moved; NEXT
; Evidence: move.b (a2)+ / ext.w / add.w d0,d4 / d1,d5; 33 corpus runs; $1C2C6 `5294 F6E3`
; ---------------------------------------------------------------------------
Op2_FxPosAddB:
	move.b	(a2)+,d0
	move.b	(a2)+,d1
	ext.w	d0
	ext.w	d1
	add.w	d0,d4
	add.w	d1,d5
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosAddW - a2 op: adds a signed word offset to the effect-spawn position d4/d5
; In:  a2 = script: word dx, word dy
; Out: d4/d5 moved; NEXT
; Evidence: add.w (a2)+,d4 / add.w (a2)+,d5; 3 corpus runs; $25DA4 `52A4 0100 0060`
; ---------------------------------------------------------------------------
Op2_FxPosAddW:
	add.w	(a2)+,d4
	add.w	(a2)+,d5
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_FxPosAddFlip - a2 op: adds a signed byte offset, mirrored by the sprite's H/V flip bits, to the effect-spawn position d4/d5
; In:  a2 = script: byte dx, byte dy
; Out: d4/d5 moved; NEXT
; Evidence: neg on btst #3/#4,$38(a5); 71 corpus runs; $1B442 `52AC 08F8`
; ---------------------------------------------------------------------------
Op2_FxPosAddFlip:
	move.b	(a2)+,d0
	move.b	(a2)+,d1
	ext.w	d0
	ext.w	d1
	btst	#$3,$38(a5)
	beq.s	loc_0052BE
	neg.w	d0
loc_0052BE:
	btst	#$4,$38(a5)
	beq.s	loc_0052C8
	neg.w	d1
loc_0052C8:
	add.w	d0,d4
	add.w	d1,d5
	movea.w	(a2)+,a0
	jmp	(a0)
