; ---------------------------------------------------------------------------
; Flight_SpawnRedNavLight - spawns a red navigation light on the airship hull: anim $2F7A0 cycles frames $42EA/$42F4 (red glow), body $2F746
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: trigger entry Tbl_0307C2+$1C at map (26, $78), the nose's upper side (its green twin Flight_SpawnGreenNavLight is at (26, $168)); frames rendered; TAS f67264
; ---------------------------------------------------------------------------
Flight_SpawnRedNavLight:
	lea.l	(Scr2_02F6DA).l,a2
	jmp	(Obj_SpawnEnemy).l
