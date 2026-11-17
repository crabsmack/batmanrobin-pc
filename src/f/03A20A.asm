; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnRoam4B - spawner of a gift box with a taller terrain box (box A height $B, not 9; weight $A) that opens into a 4-figure chain (ToyChain_MakeBox4) and roams at cruise x $2AA with low hops every $4B0 ticks
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A216 / jmp Obj_SpawnEnemy; `1DF8 0000 0011 0009 000B` (Op2_SetTerrainBoxA), `244A 002A 0003A398`; wave entries Scr5_034932+$562/+$648/+$6A4; not in the TAS: fuzz-82800-s2 f83324-f83703 (section $3100). The weight-0 variant after it ($03A264, Scr2_03A270, 10-figure toy chain $03A372) is never run, still raw
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnRoam4B:
	lea.l	(Scr2_03A216).l,a2
	jmp	(Obj_SpawnEnemy).l
