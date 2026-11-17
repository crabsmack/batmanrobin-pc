; ---------------------------------------------------------------------------
; Enemy_JetTrooper_SteerEighthTurn - native at the head of a jetpack trooper coroutine 2: target heading = heading + $80 (an eighth of a turn), wrapped to $3FF
; In:  a3 = the motion block
; Out: d0.w, $A(a3) = (heading + $80) & $3FF; back to the script
; Evidence: move.w $C(a3),d0 / addi.w #$80 / andi.w #$3FF / move.w d0,$A(a3) / jsr $7FE.w; Op2_Native at $042482 (Op2_StartCoroutine2Rel at $04247C); no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Enemy_JetTrooper_SteerEighthTurn:
	move.w	$C(a3),d0
	addi.w	#$80,d0
	andi.w	#$3FF,d0
	move.w	d0,$A(a3)
	jsr	($7FE).w
