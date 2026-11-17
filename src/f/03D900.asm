; ---------------------------------------------------------------------------
; Boss_Hatter_LittleHatSetOwner - a4 native (op $53C4) ending the little hat's spawn script: stores the owner (the boss a6 left in g_ArcTargetX by Boss_Hatter_PopLittleHat) in the new child's $12
; In:  a5 = the child just forked
; Out: $12(a5)
; Evidence: native fragment at the end of Scr4_03D8EC; rts ends the spawn script; TAS f113672 (one frame)
; ---------------------------------------------------------------------------
Boss_Hatter_LittleHatSetOwner:
	move.w	(g_ArcTargetX).w,$12(a5)
	rts
