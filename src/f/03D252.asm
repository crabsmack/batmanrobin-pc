; ---------------------------------------------------------------------------
; Boss_Hatter_StartBounce - a2 native fragment (op $07FC at $03D250) at the end of the Hatter's defeat: starts the hat's drop-and-bounce with script vars 0/1 (bounce speed, 16.16) = 0 and the drop offset (var 2 high word) = 24 px; the script then makes Boss_Hatter_StepBounce coroutine 0
; In:  a4 = the boss's a4 block
; Out: a4 +$22.l = 0, +$26 = $18
; Evidence: after BossMeter_Defeated, Hitbox_WipeEnemies and the explosion loops of Scr2_03D032; next op Op2_StartCoroutine0Here at $03D260; TAS f115654 (one frame)
; ---------------------------------------------------------------------------
Boss_Hatter_StartBounce:
	clr.l	$22(a4)
	move.w	#$18,$26(a4)
	jsr	($7FE).w
