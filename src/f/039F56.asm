; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnStay1 - spawner of a gift box (weight $A) that stays put, opens into a single toy figure (ToyChain_MakeBox1) and hops in place every $B40 ticks
; In:  d4/d5 = spawn position (trigger/spawn entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039F62 / jmp Obj_SpawnEnemy; script `0A32 000A`, `244A 002A 0003A2E6`, `17B8 0000`, timer $B40; entries Tbl_033F30+$1B0/+$1BE/+$1CC/+$1DA; TAS f96521-f96777 (4 spawns, section $3200)
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnStay1:
	lea.l	(Scr2_039F62).l,a2
	jmp	(Obj_SpawnEnemy).l
