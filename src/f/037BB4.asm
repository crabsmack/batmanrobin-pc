; ---------------------------------------------------------------------------
; ToyChain_PushByHit - native fragment of the chain variant at $037B8A: when ToyChain_TakeLinkHits reports a hit it adds the struck box's knockback +$22 to the chain's sway speed $18(a3), then re-enters the script
; In:  g_StruckBox = the struck link, a3 = motion block
; Out: $18(a3) += knockback; continues with the script after it (jsr $7FE)
; Evidence: movea.l g_StruckBox,a0 / add.l $22(a0),$18(a3); reached through the a2 op $07FC after the Op2_CallNativeRelSwitch at $037BA8; TAS f92284-f93126 (39 frames)
; ---------------------------------------------------------------------------
ToyChain_PushByHit:
	movea.l	(g_StruckBox).w,a0
	move.l	$22(a0),d0
	add.l	d0,$18(a3)
	jsr	($7FE).w
