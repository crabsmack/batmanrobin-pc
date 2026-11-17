; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnRoam1 - spawner of a light gift box (weight 1, Op2_IncPopulation) that opens into a single toy figure (ToyChain_MakeBox1) and roams at cruise x $2AA, hopping every $B40 ticks
; In:  d4/d5 = spawn position (spawn entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039E46 / jmp Obj_SpawnEnemy; script `0A26`, `244A 002A 0003A2E6`, `17B8 02AA`, timer $B40; spawn entries Tbl_034142+$30A/+$72C; TAS f108390-f112233 (section $3400); level-skip scenario f10408
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnRoam1:
	lea.l	(Scr2_039E46).l,a2
	jmp	(Obj_SpawnEnemy).l
