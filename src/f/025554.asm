; ---------------------------------------------------------------------------
; Stage2_SetSpawnVarsFromEntry - spawner tail: sets the new object's script variables 0 and 1 ($22/$24 of a4) from the two words after the callback in the trigger entry
; In:  a1 = trigger entry words, a4 = new object's script block
; Out: d0 = 0
; Evidence: jmp from Stage2_SpawnLeapingThug and $0255B2 (lea Scr2_0255C2 / Obj_SpawnEnemyOrQuit); TAS f21270-f30019
; ---------------------------------------------------------------------------
Stage2_SetSpawnVarsFromEntry:
	move.w	(a1)+,$22(a4)
	move.w	(a1)+,$24(a4)
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Stage2_SpawnLeapingThug - lift trigger callback: spawns the enemy Scr2_025570 (score $64, energy $30/$C0) and gives it script variables 0/1 from the trigger entry
; In:  a1 = after the callback long in the trigger entry
; Out: d0 = 0 (quits the caller when the pool is full)
; Evidence: dc.l at $0214A0, $0214AC, $021556 ... in Tbl_02105A; RAM f22080 and f25380: a Scr2_025570 object (vector 1 = $025596) at x 32 above the screen; shot f22100 shows a thug leaping in at the top left (identification inferred)
; ---------------------------------------------------------------------------
Stage2_SpawnLeapingThug:
	lea.l	(Scr2_025570).l,a2
	jsr	(Obj_SpawnEnemyOrQuit).l
	jmp	Stage2_SetSpawnVarsFromEntry(pc)
