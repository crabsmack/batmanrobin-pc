; ---------------------------------------------------------------------------
; Op2_LoadPalette1 - a2 op: copies the 16-colour palette at the long operand into palette buffer line 1 ($E0F2, Pal_LoadLine1, which also flags it for upload); continues
; In:  a2 = script: long palette address
; Out: NEXT
; Evidence: Pal_LoadLine1 copies 32 bytes to $E0F2 and sets $F4CE; siblings in this file: Op2_LoadPalette2 ($B6A, $E112) and Op2_LoadPalette3 ($B76, $E132), Op2_SetDepthFromY ($B82); line 0 is $E0D2 (Pal_LoadLine0); ran once in the TAS (frame 100733)
; ---------------------------------------------------------------------------
Op2_LoadPalette1:
	movea.l	(a2)+,a0
	jsr	(Pal_LoadLine1).l
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_LoadPalette2 - a2 op: copies the 16-colour palette at the long operand into palette buffer line 2 ($E112, Pal_LoadLine2, which flags it via $F4D2)
; In:  a2 = script: long palette address
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_LoadPalette2:
	movea.l	(a2)+,a0
	jsr	(Pal_LoadLine2).l
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_LoadPalette3 - a2 op: copies the 16-colour palette at the long operand into palette buffer line 3 ($E132, Pal_LoadLine3, flag $F4D6)
; In:  a2 = script: long palette address
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_LoadPalette3:
	movea.l	(a2)+,a0
	jsr	(Pal_LoadLine3).l
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetDepthFromY - a2 op: sets the sprite sort depth from the screen y: $3A(a5) = clamp((y - g_CameraY) << 5, $800..$2000) + bias, so lower objects draw in front
; In:  a2 = script: word bias
; Out: NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetDepthFromY:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	asl.w	#$5,d0
	bpl.s	loc_000B92
	move.w	#$800,d0
loc_000B92:
	cmpi.w	#$2000,d0
	ble.s	loc_000B9C
	move.w	#$2000,d0
loc_000B9C:
	add.w	(a2)+,d0
	move.w	d0,$3A(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
