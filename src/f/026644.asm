; ---------------------------------------------------------------------------
; Enemy_LadderThug_AnchorHull - the hull exit of Enemy_LadderThug_PickAnchor: resumes at $026648 (start at x $100, y -$F0 in hull space, then the hull-tilted loop $026654)
; In:  a2 script context
; Out: jsr $7FE.w
; Evidence: bne target; TAS f33179-f34742 (3 frames: ladders under the boss hull, png f033300)
; ---------------------------------------------------------------------------
Enemy_LadderThug_AnchorHull:
	jsr	($7FE).w
