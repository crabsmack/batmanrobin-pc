; ---------------------------------------------------------------------------
; Enemy_GasOrb_SpawnTripleJetsFar - spawns a tube-section gas orb with the jet-sequence / all-three-jets attacks of the faster-puff set ($43F7E / $43EEE, cruise y $1400)
; In:  d4/d5 = x/y (wave entry)
; Out: as Obj_SpawnEnemy
; Evidence: Scr2_04381E: coroutine 1 $438AC picks $43F7E or $43EEE; wave entries at $41D34/$41D3C; TAS frame 127254
; ---------------------------------------------------------------------------
Enemy_GasOrb_SpawnTripleJetsFar:
	lea.l	(Scr2_04381E).l,a2
	jmp	(Obj_SpawnEnemy).l
