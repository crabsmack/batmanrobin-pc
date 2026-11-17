; ---------------------------------------------------------------------------
; Boss_Stage2_ReadTurretsDown - native fragment of the boss's phase-1 loop: copies g_Stage2TurretsDown into script var 0 so the next op (Op2_BranchVar0Eq $F) moves to phase 2 when all four turrets are destroyed
; In:  a2 script context, a4 = boss script block
; Out: var 0 ($22(a4)) = g_Stage2TurretsDown; jsr $7FE.w
; Evidence: followed by `204E 000F` at $025C8E (branch to $025CAA: body box, damage loop); RAM: var 0 = 3 at f32700 (two turrets at energy 0), $F at f33180; TAS f32140-f33171 every frame
; ---------------------------------------------------------------------------
Boss_Stage2_ReadTurretsDown:
	move.w	(g_Stage2TurretsDown).w,$22(a4)
	jsr	($7FE).w
