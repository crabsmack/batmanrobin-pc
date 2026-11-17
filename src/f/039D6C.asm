; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnStay1Bursts - spawner of a gift box (weight 5) that stays put, opens into a single toy figure (ToyChain_MakeBox1) and hops twice ($780 apart), rests $1E00 ticks, and repeats
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039D78 / jmp Obj_SpawnEnemy; script `0A32 0005`, `244A 002A 0003A2E6`, counter 0 = 2 hops at timer $780, then timer $1E00; wave entries Scr5_034932+$16C/+$17C/+$184; TAS f91448-f91883 (4 spawns, section $3200)
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnStay1Bursts:
	lea.l	(Scr2_039D78).l,a2
	jmp	(Obj_SpawnEnemy).l
