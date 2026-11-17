; ---------------------------------------------------------------------------
; Enemy_DomeCraft_SpawnFromLeft - wave spawner: a green-domed craft (Scr2_02D024, body $280EC: energy $800, contact attack box with knockback) that enters from the left edge (sprite attr $F808) and holds windows right of the player ($2580 ticks each), then leaves right; fires 5-way fans (headings $380-$480, a4 script $298F0) and then launches 7 spinning mines upward ($299B8)
; In:  d4/d5 = x/y
; Out: as Obj_SpawnEnemy
; Evidence: Scr5_021D3A +$2D4 (x -$A, screen y $16, drop action 8); timer 1 = $3C0; Fx_SpawnAtOfsAngle x5 per volley; f44400: the craft beside the player's glider; TAS f44261
; ---------------------------------------------------------------------------
Enemy_DomeCraft_SpawnFromLeft:
	lea.l	(Scr2_02D024).l,a2
	jmp	(Obj_SpawnEnemy).l
