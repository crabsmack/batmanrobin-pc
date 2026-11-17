; ---------------------------------------------------------------------------
; Boss_Freeze_GetDamageStage - returns script variable 0, the damage stage (0-8) that the hover-platform animation's switch turns into one of nine frames
; In:  a4 = boss's a4 block
; Out: d0 = var 0 ($22(a4))
; Evidence: only caller: Op2_CallNativeRelSwitch at $046598 (nine Op2_DrawSpr cases $4844-$4894) in the block-$10 animation $046524 set at $0462BE; Scr2_046176 increments var 0 at energy 96/92/88/83/79/75/70 %; TAS 128791-130772 (every frame of the fight); platform identity from the screenshots (inferred)
; ---------------------------------------------------------------------------
Boss_Freeze_GetDamageStage:
	move.w	$22(a4),d0
	rts
