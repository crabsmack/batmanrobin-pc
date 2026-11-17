; ---------------------------------------------------------------------------
; Enemy_Brute_SpawnTough - spawns the tougher brute: palette line 2, energy $120, walks in, attack loop at $1B0CA
; In:  d4/d5 = x/y (wave script Tbl_0174F0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01AD68 = ModSprAttr $98/$40 (palette 2), SetEnergy $120, coroutine 0 $1B032; RAM f9420: an Scr2_01AF1A object with energy $120; TAS f9404
; ---------------------------------------------------------------------------
Enemy_Brute_SpawnTough:
	lea.l	(Scr2_01AD68).l,a2
	jmp	(Obj_SpawnEnemy).l
