; ---------------------------------------------------------------------------
; Enemy_Biplane_SpawnZigzagA - wave spawner: a red biplane (Scr2_02D4A2, body $28286, no attack) whose first leg has the (y, x) words ($94, $B4) and that then zigzags with legs that speed up from $4AA to $655
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_023C78 +$A0 (origin entry 0/$2000, kill-group member action 1); the rest of the course ($2D54A-$2D660) is never read; fuzz runs only
; ---------------------------------------------------------------------------
Enemy_Biplane_SpawnZigzagA:
	lea.l	(Scr2_02D4A2).l,a2
	jmp	(Obj_SpawnEnemy).l
