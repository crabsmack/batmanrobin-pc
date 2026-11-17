; ---------------------------------------------------------------------------
; Enemy_Clown_SpawnGreen - spawns the grey-green clown (palette line 2, energy $80) that drops in and picks one of three approach ranges at random
; In:  d4/d5 = x/y (Tbl_0166EA +$7F0.., Tbl_0174F0 +$14/+$1C)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01B69A = ModSprAttr $98/$40, SetEnergy $80, Op2_SwitchRandom with 3 cases; RAM f3780: a5 attr $C000 (palette 2) energy $80 on the grey-green clown of shot f3780; TAS f3515-f9504
; ---------------------------------------------------------------------------
Enemy_Clown_SpawnGreen:
	lea.l	(Scr2_01B69A).l,a2
	jmp	(Obj_SpawnEnemy).l
