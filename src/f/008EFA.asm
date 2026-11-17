; ---------------------------------------------------------------------------
; Obj_SpawnScriptedOrQuit - spawns a 4-block VM2_RunObject object at d4/d5, returning to the caller's caller when the pool is short
; In:  d4/d5 = x/y
; Out: d0 = 0, a6 = the object (caller sets the script $2A(a4) and $58); on failure pops one return address
; Evidence: callers $1F480, $1F634, $1F75C, $1F882; 89 TAS frames
; ---------------------------------------------------------------------------
Obj_SpawnScriptedOrQuit:
	bsr.w	Obj_SpawnScripted
	bne.s	loc_008F04
	moveq	#$0,d0
	rts
loc_008F04:
	addq.l	#$4,a7
	rts
