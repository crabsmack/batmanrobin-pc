; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnStay8A - spawner of a gift box (weight $A) that stays put, opens into an 8-figure swaying chain (ToyChain_MakeBox8) and hops ($28/$F0) every $4B0 ticks
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A162 / jmp Obj_SpawnEnemy; script `0A32 000A`, `244A 002A 0003A456`, `17B8 0000`, timer $4B0; wave entry Scr5_034932+$26A; TAS f83701 (section $3100)
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnStay8A:
	lea.l	(Scr2_03A162).l,a2
	jmp	(Obj_SpawnEnemy).l
