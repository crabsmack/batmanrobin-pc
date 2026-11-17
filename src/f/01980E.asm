; ---------------------------------------------------------------------------
; Boss_Harley_BranchAimReached - native fragment of the Harley boss script: when the aim $22(a5) has reached its target $24(a5) the script continues at $1981C (op $3B16 = go to the script vector $3A(a6)), else at $19822 through Boss_Harley_AimNotReached
; In:  a5 = sprite block
; Out: a2 = $1981C or $19822
; Evidence: cmp.w $22(a5),d0 / bne; the script word after the first jsr $7FE is $3B16 (movea.l $3A(a6),a2); TAS 89 frames f4980-f5075, 88 of them not reached
; ---------------------------------------------------------------------------
Boss_Harley_BranchAimReached:
	move.w	$24(a5),d0
	cmp.w	$22(a5),d0
	bne.s	Boss_Harley_AimNotReached
	jsr	($7FE).w
