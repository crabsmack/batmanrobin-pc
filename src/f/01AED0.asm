; ---------------------------------------------------------------------------
; Enemy_Brute_SpawnToughAlt - spawns the tougher brute (palette line 2, energy $120) whose attack loop starts at $1B060 (anim $2FC) instead of $1B0CA
; In:  d4/d5 = x/y (wave script Tbl_0174F0 +$D4/+$DC)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_01AEDC differs from Scr2_01AD68 only in the vector-2 +8 goto ($1B060: StartCoroutine0Rel, SetAnim $2FC, GosubVector2Plus 4); TAS f8124-f8222 (Gem Expo)
; ---------------------------------------------------------------------------
Enemy_Brute_SpawnToughAlt:
	lea.l	(Scr2_01AEDC).l,a2
	jmp	(Obj_SpawnEnemy).l
