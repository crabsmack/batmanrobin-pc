; ---------------------------------------------------------------------------
; Boss_Hatter_SpawnLittleHat - spawns the little top hat Scr2_03D940 (boss class, energy $120): it jumps out of the defeated hat (Op2_JumpAnim), bounces along the road, and when hit drops an extra life (Boss_Hatter_LittleHatDropLife)
; In:  d4/d5 = position (Spawn_AtOwnOfs: the boss + (0, -20))
; Out: a6 = the object (Obj_SpawnBoss)
; Evidence: Spawn_AtOwnOfs dc.l $0003D934 at $03D74A in Scr2_03D508, entered by jsr (a0) at $008DAC; TAS f115594 (one frame); png f115700: a small black top hat with a blue band on the road
; ---------------------------------------------------------------------------
Boss_Hatter_SpawnLittleHat:
	lea.l	(Scr2_03D940).l,a2
	jmp	(Obj_SpawnBoss).l
