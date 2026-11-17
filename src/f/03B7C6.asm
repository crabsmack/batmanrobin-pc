; ---------------------------------------------------------------------------
; Boss_CatHead_EyeFxAt - the four eye natives' shared tail: the eye point (d0 + the head's x, -$F + its y) through the sway, heading = the head's + the shear x 256 + the script's word, then Fx_RunScript with the a4 script a0
; In:  d0.w = the eye's x offset, a0 = the a4 script; a6/a3/a2 as Boss_CatHead_EyeSparkLeft
; Out: a2 past the word; d1 = the eye y, d2 = 0, d3 = the heading, d4/d5 = the position (16.16); then Fx_RunScript's
; Evidence: move.w #$FFF1,d1 / add.w $3E(a6),d0 / add.w $42(a6),d1 / bsr.s Boss_CatHead_SwayPoint / swap d4 / swap d5 / moveq #0,d2 / move.w (g_SceneVars+$10).w,d3 / ext.l / asl.l #8 / add.l $C(a3) / swap / add.w (a2)+ / swap / jmp Fx_RunScript; the bra.s of Boss_CatHead_EyeSparkLeft / Right and the fall-through of Boss_CatHead_EyeShotAt; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_EyeFxAt:
	move.w	#$FFF1,d1
	add.w	$3E(a6),d0
	add.w	$42(a6),d1
	bsr.s	Boss_CatHead_SwayPoint
	swap	d4
	swap	d5
	moveq	#$0,d2
	move.w	(g_SceneVars+$10).w,d3
	ext.l	d3
	asl.l	#$8,d3
	add.l	$C(a3),d3
	swap	d3
	add.w	(a2)+,d3
	swap	d3
	jmp	(Fx_RunScript).l
