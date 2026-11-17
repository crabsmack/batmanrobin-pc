; ---------------------------------------------------------------------------
; Boss_Freeze_ShotRecoilLeft - native step of Mr. Freeze's unflipped (facing right) firing animation: x impulse $2(a3) = -3 (recoil), the twin of Boss_Freeze_ShotRecoilRight
; In:  a3 = the boss's motion block
; Out: back to the animation at $0467CA
; Evidence: move.w #-3,$2(a3) / jsr $7FE.w; Op2_Native at $0467BE; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_Freeze_ShotRecoilLeft:
	move.w	#$FFFD,$2(a3)
	jsr	($7FE).w
