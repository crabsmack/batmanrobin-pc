; ---------------------------------------------------------------------------
; WeaponDrop_FromTopRandom - director native: drops a pickup from above the screen at a random x (Rand_Signed($122) + $A0 from the screen's left, y = -10), then falls into WeaponDrop_SpawnOnScreen
; In:  a2 = script at the dc.l spawner
; Out: as WeaponDrop_SpawnOnScreen
; Evidence: Op2_CallNativeRel at $01406E (dc.l Item_SpawnPowerUpFalling after it) in the weapon-drop director script; move.w #$122,d0 / jsr Rand_Signed / move.w d0,d4 / addi.w #$A0,d4 / moveq #-10,d5, falling into WeaponDrop_SpawnOnScreen; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
WeaponDrop_FromTopRandom:
	move.w	#$122,d0
	jsr	(Rand_Signed).l
	move.w	d0,d4
	addi.w	#$A0,d4
	moveq	#-$A,d5
