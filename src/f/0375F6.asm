; ---------------------------------------------------------------------------
; ToyRing_DoubleKnockback - native fragment of the toy-ring body (Scr2_037588): after Op2_TakeHit it doubles the x impulse $2(a3) so a hit ring is thrown back twice as far, then re-enters the script
; In:  a3 = motion block
; Out: $2(a3) = 2 x impulse; continues with the a2 script after it (jsr $7FE)
; Evidence: add.l d0,d0 on $2(a3); reached through the a2 op $07FC (jmp (a2)) after Op2_TakeHit $32 at $0375F0; TAS f79739-f112908 (57 frames)
; ---------------------------------------------------------------------------
ToyRing_DoubleKnockback:
	move.l	$2(a3),d0
	add.l	d0,d0
	move.l	d0,$2(a3)
	jsr	($7FE).w
