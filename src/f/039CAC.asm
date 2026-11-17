; ---------------------------------------------------------------------------
; Enemy_GiftBox_SpawnRoamWide10 - spawner of a hopping gift box (Scr2_039CB8 on the box body Scr2_037C02, weight $A): after $1E0+$1E0 ticks it bursts open and lets out a 10-figure toy chain with a wide sway (ToyChain_MakeBox10Wide), then roams at cruise x $2AA, hopping (Op2_JumpAnim $32/$F0) every $B40 ticks
; In:  d4/d5 = spawn position (wave entry)
; Out: Obj_SpawnEnemy's result
; Evidence: lea Scr2_039CB8 / jmp Obj_SpawnEnemy; script `0A32 000A`, SetAnim (open), `244A 002A 0003A34C`, `17B8 02AA`, `1C8C 0032 00F0`, timer $B40; spawn entries Tbl_034142+$31A/+$73C; TAS f108694-f112170 (section $3400 card hall); box frames $45D8 (wrapped present with a ribbon) and $45E2-$461E (opening/burst) rendered from Tbl_1DDB60
; ---------------------------------------------------------------------------
Enemy_GiftBox_SpawnRoamWide10:
	lea.l	(Scr2_039CB8).l,a2
	jmp	(Obj_SpawnEnemy).l
