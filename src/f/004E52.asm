; ---------------------------------------------------------------------------
; Hitbox_Strike - one hit: tags the struck box with the attacker's owner id and runs the attack box's hit callback, then the struck box's callback
; In:  a0 = attack box, a2 = struck box (both with a callback long at +$30); a6 = attacker
; Out: whatever the callbacks return (tail jump into the struck box's)
; Evidence: move.w $58(a6),$1A(a2) / movea.l $30(a0),a1 / jsr (a1) / movea.l $30(a2),a1 / jmp (a1); callbacks such as PlayerAtk_HitPause ($3784 attack spec); 76 corpus runs
; ---------------------------------------------------------------------------
Hitbox_Strike:
	move.w	$58(a6),$1A(a2)
	movea.l	$30(a0),a1
	jsr	(a1)
	movea.l	$30(a2),a1
	jmp	(a1)
; ---------------------------------------------------------------------------
; Hitbox_StrikeAll - as Hitbox_Strike for every remaining overlapping box (Hitbox_FindNextBox) on the list
; In:  a0 = attack box, a2 = first struck box; d4-d7 = rectangle; a6 = attacker
; Out: rts after the last overlapping box
; Evidence: same callback pair, then bsr Hitbox_FindNextBox / bne loop
; ---------------------------------------------------------------------------
Hitbox_StrikeAll:
	move.w	$58(a6),$1A(a2)
	movea.l	$30(a0),a1
	jsr	(a1)
	movea.l	$30(a2),a1
	jsr	(a1)
	bsr.w	Hitbox_FindNextBox
	bne.s	Hitbox_StrikeAll
	rts
