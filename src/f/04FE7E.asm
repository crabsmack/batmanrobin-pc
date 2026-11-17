; ---------------------------------------------------------------------------
; Player_CopyPadToVar2Robin - native fragment of Robin's script in the jetpack tunnel sections: copies script var 0 (the held pad bits) to var 2, as Batman's Player_CopyPadToVar2
; In:  a4 = script block
; Out: var 2 ($26(a4)) = var 0 ($22(a4)); back to the script
; Evidence: move.w $22(a4),$26(a4) / jsr $7FE.w; Op2_Native at $04FE7C in Robin's script after BranchVar1None $20 / AndVar1 $FFDF; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Player_CopyPadToVar2Robin:
	move.w	$22(a4),$26(a4)
	jsr	($7FE).w
