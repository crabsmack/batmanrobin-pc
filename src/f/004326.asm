; ---------------------------------------------------------------------------
; Op2_SetBlinkTimer - a2 op: sets the sprite's blink timer (byte $16(a5)): while it runs Op2_Draw skips drawing on alternate frames (the player's spawn invulnerability uses the same byte)
; In:  a2 = script: word frames (low byte used)
; Out: NEXT
; Evidence: move.b d0,$16(a5); Op2_Draw: subq.b #1,$16(a5) / tst.b g_FrameParity / bne $7E0; $101CE sets $16(a5) = $78 on spawn and Hitbox_CanHitPlayer tests it; 132 corpus runs; $132EA `4326 0005`
; ---------------------------------------------------------------------------
Op2_SetBlinkTimer:
	move.w	(a2)+,d0
	move.b	d0,$16(a5)
	movea.w	(a2)+,a0
	jmp	(a0)
