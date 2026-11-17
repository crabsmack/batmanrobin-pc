; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnRoam1HopFast - as Enemy_GiftBox_SpawnRoam1 (weight 1, one figure, cruise x $2AA) but hopping every $5A0 ticks
; In:  d4/d5 = spawn position (spawn entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039EA4 / jmp Obj_SpawnEnemy; script `0A26`, `244A 002A 0003A2E6`, `17B8 02AA`, timer $5A0; spawn entries Tbl_034142+$312/+$734; TAS f108390-f112233 (section $3400). The weight-$A stay-put variant after it ($039EF6, Scr2_039F02, hop every $780) is never run, still raw
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnRoam1HopFast:
	lea.l	(Scr2_039EA4).l,a2
	jmp	(Obj_SpawnEnemy).l
