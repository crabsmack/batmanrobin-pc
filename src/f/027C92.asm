; ---------------------------------------------------------------------------
; Spawn_SetTimer0FromEntry - spawner tail: sets the new object's tick timer 0 ($1A(a4)) from the next word of the trigger/wave entry
; In:  a1 = entry words after the callback, a4 = new object's script block
; Out: d0 = 0, a1 past the word
; Evidence: jmp operands at $03130A, $03179E, $0317F6 in inline spawners (lea Scr2_x / Obj_SpawnEnemyOrQuit); TAS f67325-f75173 (37)
; ---------------------------------------------------------------------------
Spawn_SetTimer0FromEntry:
	move.w	(a1)+,$1A(a4)
	moveq	#$0,d0
	rts
; ---------------------------------------------------------------------------
; Spawn_SetTimer1FromEntry - spawner tail: sets the new object's tick timer 1 ($1C(a4)) from the next word of the trigger/wave entry (the flying-section body $027CA2 gosubs vector 1 when it runs out)
; In:  a1 = entry words after the callback, a4 = new object's script block
; Out: d0 = 0, a1 past the word
; Evidence: jmp from 30+ flying-section spawners ($029EFC, $029F7A, $02A146 ... and the jmp operands at $0312A8, $03159C, $0315FC)
; ---------------------------------------------------------------------------
Spawn_SetTimer1FromEntry:
	move.w	(a1)+,$1C(a4)
	moveq	#$0,d0
	rts
