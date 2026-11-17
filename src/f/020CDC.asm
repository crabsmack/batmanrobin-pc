; ---------------------------------------------------------------------------
; Stage1_SpawnBankBombing - street trigger callback: ends all waves and spawns the bank bombing director Scr2_020CF6, which keeps the entry's extra long (the wave script to start afterwards) in $38
; In:  a1 = after the callback long in the trigger entry
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: dc.l $00020CDC at $016146 in Tbl_015F5A; Wave_EndAll; TAS f1729. The director waits for no enemies, then explosions sweep the upper floor (f1760), both signs blast, rubble falls, the big blast lights the window fire (f2116)
; ---------------------------------------------------------------------------
Stage1_SpawnBankBombing:
	lea.l	(Scr2_020CF6).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	move.l	(a1)+,$38(a6)
	jsr	(Wave_EndAll).l
	moveq	#$0,d0
	rts
