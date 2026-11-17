; ---------------------------------------------------------------------------
; Enemy_PoleThug_CheckDropY - pole-thug native switch run every frame of its motion coroutine: 1 once its y reaches script var 1 (jump off the pole to the floor), 2 once it reaches var 0 (var 0 then set to $7FFF so it fires once), else 0
; In:  a6 = thug (+$42 y), a4 = its script block (+$22 var 0, +$24 var 1)
; Out: d0 = 0 / 1 / 2 for Op2_CallNativeRelSwitch
; Evidence: only caller the op $2D30 at $025820 (cases: $02582A return, $02592E start hit coroutines + Op2_JumpAnim + land, $02589E Op2_GotoVector2); every spawn entry gives var 0 = $7FFF, so case 2 never fires in the corpus; TAS f21270-f30032 (1626 frames)
; ---------------------------------------------------------------------------
Enemy_PoleThug_CheckDropY:
	move.w	$42(a6),d0
	cmp.w	$24(a4),d0
	bge.s	loc_0258B4
	cmp.w	$22(a4),d0
	bge.s	loc_0258B8
	moveq	#$0,d0
	rts
loc_0258B4:
	moveq	#$1,d0
	rts
loc_0258B8:
	move.w	#$7FFF,$22(a4)
	moveq	#$2,d0
	rts
