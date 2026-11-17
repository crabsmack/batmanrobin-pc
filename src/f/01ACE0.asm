; ---------------------------------------------------------------------------
; Enemy_Brute_SpawnFromSign - spawns the big sack-carrying brute that leaps out of a blasted bank sign: palette line 3, energy $D0, its entry coroutine starts with a jump (height $23, time $F0)
; In:  d4/d5 = x/y (trigger entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01ACEC = ModSprAttr $98/$60, SetScoreValue $96, SetEnergy $D0, coroutine 0 = $1B012 (Op2_JumpAnim $23,$F0), then the shared brute body $1AF74 (vector 2); Tbl_015F5A entries chain Stage1_BlastSignChained -> this; TAS f2896-f4284, shot f2940 (brute in front of the blasted sign)
; ---------------------------------------------------------------------------
Enemy_Brute_SpawnFromSign:
	lea.l	(Scr2_01ACEC).l,a2
	jmp	(Obj_SpawnEnemy).l
