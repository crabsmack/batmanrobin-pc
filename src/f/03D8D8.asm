; ---------------------------------------------------------------------------
; Boss_Hatter_PopLittleHat - a2 native (op $2CBC) in the hat's animation: makes a little top hat pop up out of the big hat and fall back in: runs Scr4_03D8EC (forks Scr4_03D908 20 px up, rising 1 px per frame under gravity 0.1) at the boss's y, passing the boss's a6 through g_ArcTargetX for Boss_Hatter_LittleHatSetOwner
; In:  a6 = the boss
; Out: g_ArcTargetX = the boss's a6 (word), as Fx_RunScript
; Evidence: dc.l $0003D8D8 at $03D6F4 in Scr2_03D508; the child draws RAM frame $FDD0, the same sprite as Boss_Hatter_SpawnLittleHat's object (png f115700: a small black top hat on the road); TAS f113672 (one frame)
; ---------------------------------------------------------------------------
Boss_Hatter_PopLittleHat:
	move.w	a6,(g_ArcTargetX).w
	lea.l	(Scr4_03D8EC).l,a0
	move.l	$42(a6),d5
	jmp	(Fx_RunScript).l
