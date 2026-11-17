; ---------------------------------------------------------------------------
; Boss_Hatter_RoadCmdTbl - head of the road schedule's command jump table: 16 bra.s (2 bytes each) entered by `jmp (pc,d0.w)` at $03DCCA with d0 = command nibble x 2; this first entry (command 0) branches to the end-of-schedule code (clears the stack depth, the wait and slot 2)
; In:  a0 = past the command, d4 = byte 0, d1 = byte 1
; Out: as the command
; Evidence: raw bra.s targets $3DCCE (end), $3DCDC (wait), $3DD02 (goto), $3DCF4 (gosub), $3DD0E (return), $3DCE6 (set loop), $3DCEC (loop), $3DD1E (random), then $3DD52-$3DD8A (spawn kinds 0-7); ctarget in 3 corpus runs
; ---------------------------------------------------------------------------
Boss_Hatter_RoadCmdTbl:
	bra.s	Boss_Hatter_RoadCmdEnd
