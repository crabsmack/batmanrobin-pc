; ---------------------------------------------------------------------------
; Player_CopyPadToVar2 - native fragment of Batman's script in the jetpack tunnel sections: copies script var 0 (the held pad bits) to var 2, which the next ops switch on to pick the pose
; In:  a4 = script block, a2 = script after the Op2_Native op
; Out: var 2 ($26(a4)) = var 0 ($22(a4)); back to the script
; Evidence: move.w $22(a4),$26(a4) / jsr $7FE; inside Scr2_04ACD0 (Batman) after BranchVar1None $20 / AndVar1 $FFDF (taken only on a pad release), followed by Scr2_04D34C Op2_SwitchVar2Bits -> SetAnim; TAS f101718-130436 (255 frames), png f102000 (Batman with a jetpack in the red tunnel), f126000 (the blue ring tunnel)
; ---------------------------------------------------------------------------
Player_CopyPadToVar2:
	move.w	$22(a4),$26(a4)
	jsr	($7FE).w
