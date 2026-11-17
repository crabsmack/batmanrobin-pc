; ---------------------------------------------------------------------------
; Op2_BranchWeaponLvlMaxGE - a2 op: branches when g_WeaponLvlMax (the best weapon level among players in play) is at least the operand
; In:  a2 = script: word level, word offset
; Out: branch, else NEXT after the operands
; Evidence: move.w $F638,d0 / bra Op2_BranchStatGE; its never-run neighbours $3CDC-$3D4E compare the other Player_GatherStats words; 38 corpus runs; $13FD4 `3CEE 0002 FFEC`
; ---------------------------------------------------------------------------
Op2_BranchWeaponLvlMaxGE:
	move.w	(g_WeaponLvlMax).w,d0
	bra.s	Op2_BranchStatGE
