; ---------------------------------------------------------------------------
; Boss_Freeze_ShotRecoilDown - native step of one of Mr. Freeze's firing animations: y impulse $6(a3) = +3 (the recoil pushes him down)
; In:  a3 = the boss's motion block
; Out: back to the animation at $0468A8
; Evidence: move.w #3,$6(a3) / jsr $7FE.w; Op2_Native at $04689C, then Fade_InFromPale6 and the shot as the Right twin; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Freeze_ShotRecoilDown:
	move.w	#$3,$6(a3)
	jsr	($7FE).w
