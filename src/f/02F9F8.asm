; ---------------------------------------------------------------------------
; Enemy_HullTurret_SpinStep - native coroutine step of the spinning hull turret: target heading = current heading + $80 (45 degrees clockwise), then back into the script, which turns toward it (Op2_TurnHeading) and returns
; In:  a3 = the turret's motion block
; Out: $A(a3) set; re-enters the script (jsr $7FE)
; Evidence: Op2_Native $07FC at $2F9F6, the coroutine-0 target ($31252, $311E2, $312B6, $31378) of the spinning turret spawners; TAS 68275-73904 (108 frames)
; ---------------------------------------------------------------------------
Enemy_HullTurret_SpinStep:
	move.w	$C(a3),d0
	addi.w	#$80,d0
	andi.w	#$3FF,d0
	move.w	d0,$A(a3)
	jsr	($7FE).w
