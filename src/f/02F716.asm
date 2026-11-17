; ---------------------------------------------------------------------------
; Flight_SpawnGreenNavLightFlip - spawns the green navigation light H-flipped (Op2_ModSprAttr $9008, the green anim $2F7BA, body $2F746)
; In:  d4/d5 = x/y (the airship path entry's)
; Out: as Obj_SpawnEnemy (d0 = 0 and a6 = the new object; nonzero d0 when the pool is short)
; Evidence: spawner long(s) at $0FDA7A; lea script / jmp Obj_SpawnEnemy as Flight_SpawnRedNavLightFlip; raw until crane-crash (2026-10-30): no corpus run spawns it
; ---------------------------------------------------------------------------
Flight_SpawnGreenNavLightFlip:
	lea.l	(Scr2_02F722).l,a2
	jmp	(Obj_SpawnEnemy).l
