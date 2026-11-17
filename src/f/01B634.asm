; ---------------------------------------------------------------------------
; Enemy_Clown_SpawnWeak - spawns a striped-shirt clown with the low energy $20 (palette line 3) that drops in from above
; In:  d4/d5 = x/y (street trigger Tbl_015F5A, also Tbl_0166EA +$2B0)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01B640 = ModSprAttr $98/$60, score $64, SetEnergy $20, coroutine 0 $1BAD6 (op $1CDC: FallAnim without the 1-px nudge, then anim $258); street clowns (energy 32, waiting above the screen at y < 0) in RAM f2160-f2220; shot f1260 (clowns in pointed hats); TAS f622-f9231 (23 frames)
; ---------------------------------------------------------------------------
Enemy_Clown_SpawnWeak:
	lea.l	(Scr2_01B640).l,a2
	jmp	(Obj_SpawnEnemy).l
