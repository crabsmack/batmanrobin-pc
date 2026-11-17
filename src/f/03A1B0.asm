; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnStay8B - second copy of Enemy_GiftBox_SpawnStay8A (Scr2_03A1BC has the same ops and the same chain $03A456, only its own address), used by the next wave entry
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A1BC / jmp Obj_SpawnEnemy; script `0A32 000A`, `244A 002A 0003A456`, timer $4B0; wave entry Scr5_034932+$272; TAS f83732 (section $3100)
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnStay8B:
	lea.l	(Scr2_03A1BC).l,a2
	jmp	(Obj_SpawnEnemy).l
