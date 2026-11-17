; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpinJets - a2 native fragment (op $07FC at $041E98) in the gas orb's body $041E4A: turns the heading of its second motion block ($E(a6), loaded by Op2_LoadA3Handle) by twice that block's x move this frame, so the orb's three gas jets (placed 120 degrees apart around that heading by Enemy_GasOrb_RunJets) rotate as it moves sideways
; In:  a3 = the motion block at $E(a6), d1 = its x move (16.16, from Op2_StepVelocity)
; Out: a3 +$C heading
; Evidence: add.l d1,d1 / add.l d1,$C(a3) after Op2_LoadA3Handle $E and Op2_StepVelocity; Enemy_GasOrb_RunJets adds $155 per jet to the same heading word; TAS f124189-f130140 (1913 frames, stage 4 tube section and Freeze fight)
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpinJets:
	add.l	d1,d1
	add.l	d1,$C(a3)
	jsr	($7FE).w
