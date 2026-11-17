; ---------------------------------------------------------------------------
; Flight_SpawnRedNavLightFlip - spawns the red navigation light H-flipped (attr xor $08, anim $2F7A0, body $2F746)
; In:  d4/d5 = trigger x/y (map space)
; Out: Obj_SpawnEnemy's result
; Evidence: two trigger entries in Tbl_0307C2 at map ($ECC, $CB/$115), the far end of the hull; TAS f75442
; ---------------------------------------------------------------------------
Flight_SpawnRedNavLightFlip:
	lea.l	(Scr2_02F6F2).l,a2
	jmp	(Obj_SpawnEnemy).l
