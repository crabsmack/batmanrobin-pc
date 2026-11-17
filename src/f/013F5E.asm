; ---------------------------------------------------------------------------
; WeaponDrop_FromTop - director native: drops a pickup from above the screen at a random x from the operand table (+$A0 from the screen centre, y = -10)
; In:  a2 = script: word count n, n word x offsets, dc.l spawner
; Out: as WeaponDrop_SpawnOnScreen; a2 past the operands
; Evidence: op $2CB2 in the director scripts at $13FDA/$14002 with offsets -$80/+$80 and Item_SpawnPowerUpFalling; Rand_Below(n); ran in scenario-continue/game-over
; ---------------------------------------------------------------------------
WeaponDrop_FromTop:
	move.w	(a2)+,d0
	move.w	d0,d1
	jsr	(Rand_Below).l
	add.w	d0,d0
	add.w	d1,d1
	move.w	(a2,d0.w),d4
	adda.w	d1,a2
	addi.w	#$A0,d4
	move.w	#$FFF6,d5
	bra.s	WeaponDrop_SpawnOnScreen
; ---------------------------------------------------------------------------
; WeaponDrop_FromRight - director native: spawns a pickup just past the right screen edge (x = $14A) at a random height ($70 +/- $5F)
; In:  a2 = script at the dc.l spawner
; Out: as WeaponDrop_SpawnOnScreen
; Evidence: op $2CB2 at $1402A/$1404C with Item_SpawnPowerUpDrifting; Rand_Signed($BE); code read (interior entry), level-skip scenario runs the drifting power-up
; ---------------------------------------------------------------------------
WeaponDrop_FromRight:
	move.w	#$BE,d0
	jsr	(Rand_Signed).l
	move.w	d0,d5
	move.w	#$14A,d4
	addi.w	#$70,d5
	bra.s	WeaponDrop_SpawnOnScreen
