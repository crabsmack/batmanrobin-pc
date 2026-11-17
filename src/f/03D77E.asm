; ---------------------------------------------------------------------------
; Boss_Hatter_SwitchCruiseDir - switch for the hat's animation script: d0 = 1/2 when the target x speed a3 +$24 is negative/positive, else 3/4 for the target y speed a3 +$2C negative/positive, 0 when both are 0
; In:  a3 = the boss's motion block
; Out: d0 = 0-4 (Op2_CallNativeRelSwitch)
; Evidence: Op2_CallNativeRelSwitch $03D77E five times in the hat's animation script Scr2_03D79E ($03D7C0-$03D8B8); TAS f113356-f115351 (92 frames)
; ---------------------------------------------------------------------------
Boss_Hatter_SwitchCruiseDir:
	move.w	$24(a3),d0
	bne.s	loc_03D794
	move.w	$2C(a3),d0
	beq.s	loc_03D79C
	bpl.s	loc_03D790
	moveq	#$3,d0
	rts
loc_03D790:
	moveq	#$4,d0
	rts
loc_03D794:
	bpl.s	loc_03D79A
	moveq	#$1,d0
	rts
loc_03D79A:
	moveq	#$2,d0
loc_03D79C:
	rts
