; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnStay10A - spawner of a gift box that stays put (cruise 0, weight 5): opens after $3C0 ticks into a 10-figure swaying chain (ToyChain_MakeBox10A) and then hops in place every $780 ticks
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039D18 / jmp Obj_SpawnEnemy; script `0A32 0005`, `244A 002A 0003A2C0`, `17B8 0000`, loop timer $780 + `1C8C 0032 00F0`; wave entries Scr5_034932+$A2/+$174; TAS f80169-f91145 (3 spawns, sections $3100-$3200); png shot f80700: the chain of figures curled into a ring above an opened box
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnStay10A:
	lea.l	(Scr2_039D18).l,a2
	jmp	(Obj_SpawnEnemy).l
