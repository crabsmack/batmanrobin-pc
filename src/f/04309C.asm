; ---------------------------------------------------------------------------
; Enemy_GunTank_ClampAimUp - native fragment of the gun tank's turret coroutine 3: after Op2_AimAtTracked keeps the target heading $A(a3) in the upper half (lower-left aims become $202, lower-right $3FE), then resumes the script (Op2_Return at $430C2)
; In:  a3 = turret motion block (+$A target heading 0-$3FF, $100 = down)
; Out: re-enters the script with jsr $7FE
; Evidence: word $07FC at $4309A after `2414 0004 2D2A 1528` (Op2_StartCoroutine3Rel, Op2_AimAtTracked) in the gun tank body $42FEC; headings ($100,$200] -> $202, [0,$100] -> $3FE; TAS 922 frames 119574-120799 (stage 4 lab, the domed tanks with a twin gun, shot w29_s119760)
; ---------------------------------------------------------------------------
Enemy_GunTank_ClampAimUp:
	move.w	$A(a3),d0
	andi.w	#$3FF,d0
	cmpi.w	#$200,d0
	bgt.s	loc_0430BE
	cmpi.w	#$100,d0
	ble.s	loc_0430B6
	move.w	#$202,d0
	bra.s	loc_0430BA
loc_0430B6:
	move.w	#$3FE,d0
loc_0430BA:
	move.w	d0,$A(a3)
loc_0430BE:
	jsr	($7FE).w
