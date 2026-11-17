; ---------------------------------------------------------------------------
; Flight_SpawnGreenNavLight - spawns a green navigation light on the airship hull: anim $2F7BC cycles frames $42FE/$4308 (green glow), body $2F746
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2+$2A at map (26, $168); frames rendered; TAS f67264. Raw, never run: an H-flipped twin at $2F716 (Scr2_02F722)
; ---------------------------------------------------------------------------
Flight_SpawnGreenNavLight:
	lea.l	(Scr2_02F70A).l,a2
	jmp	(Obj_SpawnEnemy).l
