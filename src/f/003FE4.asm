; ---------------------------------------------------------------------------
; Op2_AnimFrame - animation-script op: shows a frame: points the sprite's draw list $2A(a5) at the inline block after the offset word (sprite ops ended by Op2_DrawEnd), continues the animation at the offset next time and holds for the display time $1C(a5)
; In:  a2 = animation script: word offset past the inline draw block
; Out: $2A(a5) = block, $1E(a5) += $1C(a5), a2 = target, rts to Op2_RunAnim
; Evidence: movea.l a2,a0 / adda.w (a0)+,a2 / move.l a0,$2A(a5) / add.w d0,$1E(a5) / rts; Op2_Draw runs $2A(a5); 1664 script uses, e.g. $1345E `3FE4 0008 4524 2A80 44F2`; 153 runs
; ---------------------------------------------------------------------------
Op2_AnimFrame:
	movea.l	a2,a0
	adda.w	(a0)+,a2
	move.l	a0,$2A(a5)
	move.w	$1C(a5),d0
	add.w	d0,$1E(a5)
	rts
