; ---------------------------------------------------------------------------
; Boss_CatHead_TakeDamage - native fragment of the cat head's hit test: takes the damage of the struck box (g_StruckBox +$20, cleared), a quarter of it unless the striker id (+$1A) is 1 or 2 (a player's own attack, not a shot), from the energy $52(a6); at 0 resumes the script at $03B28C (goto the defeat at $03B2CE), else falls into Boss_CatHead_DamageSurvived
; In:  a6 = the head object, g_StruckBox = the box Op2_BranchNotStruck found
; Out: $52(a6) lowered (floored at 0); re-enters the a2 script by jsr $7FE
; Evidence: dropped into after Op2_BranchNotStruck / Tint_FlashC1 at $03B254 (a2 op $07FC); TAS f87527-f89343 (180 frames, the last one the energy hit 0: RAM f89640 energy 0)
; ---------------------------------------------------------------------------
Boss_CatHead_TakeDamage:
	movea.l	(g_StruckBox).w,a0
	move.w	$20(a0),d0
	clr.w	$20(a0)
	move.w	$1A(a0),d1
	cmpi.w	#$1,d1
	beq.s	loc_03B27E
	cmpi.w	#$2,d1
	beq.s	loc_03B27E
	lsr.w	#$2,d0
loc_03B27E:
	sub.w	d0,$52(a6)
	bgt.s	Boss_CatHead_DamageSurvived
	clr.w	$52(a6)
	jsr	($7FE).w
