; ---------------------------------------------------------------------------
; Boss_Freeze_ShotRecoilRight - native step of Mr. Freeze's flipped (facing left) firing animation: x impulse $2(a3) = +3 (recoil), then the animation flashes the palette (Fade_InFromPale6) and spawns the shot effect Scr4 $014A4C
; In:  a3 = boss's motion block; entered by Op2_Native ($07FC) at $046828
; Out: re-enters the animation script at $046834 (jsr $7FE)
; Evidence: the bytes after it: 2CBC Fade_InFromPale6, 51F2 E0F0 $00014A4C, frames with attr $F808; the unflipped twin at $0467BE (impulse -3) is still raw and never ran; TAS f130232 only
; ---------------------------------------------------------------------------
Boss_Freeze_ShotRecoilRight:
	move.w	#$3,$2(a3)
	jsr	($7FE).w
