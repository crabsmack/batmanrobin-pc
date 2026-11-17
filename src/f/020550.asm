; ---------------------------------------------------------------------------
; Stage1_TrashCanHasDrop - trash-can native switch: returns 1 when the can carries a spawn action (a drop) in $19(a6), else 0, so its death script spawns the drop effect first
; In:  a6 = trash can
; Out: d0 = 0 / 1
; Evidence: Op2_CallNativeRelSwitch at $020540 in Scr2_020504 (case 1 = Op2_FxSpawnHere $02059C, then Op2_AwardScoreToHitter); TAS f603-f4385, two-player scenario f1722
; ---------------------------------------------------------------------------
Stage1_TrashCanHasDrop:
	move.b	$19(a6),d0
	ext.w	d0
	beq.s	loc_02055A
	moveq	#$1,d0
loc_02055A:
	rts
