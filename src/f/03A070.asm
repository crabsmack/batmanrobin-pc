; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnRoamRandom - spawner of a gift box (weight $A) whose chain is picked at random when it opens (subroutine $03A11C, Op2_SwitchRandom: 4, 4, 5 or 6 figures, the ToyChain_MakeBox4 family with sways $D5/$12A/$180/$1D5), then roams at cruise x $2AA with low hops every $4B0 ticks
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_03A07C / jmp Obj_SpawnEnemy; `2D10` gosub to $03A11C (`09F4 C000 ... ` + `244A 002A 0003A3BE/0003A3E4/0003A40A/0003A430`); wave entries Scr5_034932+$45E..+$486 (6); TAS f83521 (section $3100). The variant after it ($03A0C6, Scr2_03A0D2, taller box) is never run, still raw
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnRoamRandom:
	lea.l	(Scr2_03A07C).l,a2
	jmp	(Obj_SpawnEnemy).l
