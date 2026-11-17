; ---------------------------------------------------------------------------
; Boss_Freeze_ShotRecoilUp - native step of one of Mr. Freeze's firing animations: y impulse $6(a3) = -3 (the recoil pushes him up)
; In:  a3 = the boss's motion block
; Out: back to the animation at $0468FE
; Evidence: move.w #-3,$6(a3) / jsr $7FE.w; Op2_Native at $0468F2; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Freeze_ShotRecoilUp:
	move.w	#$FFFD,$6(a3)
	jsr	($7FE).w
