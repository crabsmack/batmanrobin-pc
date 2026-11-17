; ---------------------------------------------------------------------------
; Enemy_GasOrb_RunJets - runs the gas orb's three gas jets 120 degrees apart: jet 1/2/3 = var 3 bit 1/2/4, puff block handles in $12/$14/$16(a6), the jet heading $C(a3) turned by $155 between them and restored
; In:  a6 = orb object, a3 = the jets' own motion block $E(a6), loaded by Op2_LoadA3Handle $E at $41E8E (+$C heading, +$28/$34 puff accel/speed), a4 = script block (+$28 var 3 jet mask), a5 = sprite block
; Out: a2 preserved; d0-d7/a0/a1 clobbered
; Evidence: called by Op2_CallNativeRel at $41EAA in the orb's frame loop Scr2_041EA4; the attack scripts ($43E1C-$4437E) set the jets with Op2_OrVar3/AndVar3 (5 = jets 1+3, 7 = all three); RAM f129360: var 3 = 7 and three white puff jets around the purple red-eyed orb (shot w29_s129360); TAS 124189-130140 (1917 frames: stage 4 tube section and the Mr. Freeze fight)
; ---------------------------------------------------------------------------
Enemy_GasOrb_RunJets:
	move.l	a2,-(a7)
	lea.l	$12(a6),a2
	move.w	#$1,d0
	bsr.w	Enemy_GasOrb_StepJet
	addi.w	#$155,$C(a3)
	lea.l	$14(a6),a2
	move.w	#$2,d0
	bsr.w	Enemy_GasOrb_StepJet
	addi.w	#$155,$C(a3)
	lea.l	$16(a6),a2
	move.w	#$4,d0
	bsr.w	Enemy_GasOrb_StepJet
	subi.w	#$2AA,$C(a3)
	movea.l	(a7)+,a2
	rts
