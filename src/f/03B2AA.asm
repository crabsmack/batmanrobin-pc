; ---------------------------------------------------------------------------
; Boss_CatHead_PublishPos - native fragment of the cat head's frame loop: stores the head's x/y ($3E/$42(a6)) in g_SceneVars+8/+$A, where Boss_CatHead_FrameScroll scrolls plane A (the face picture) to follow it
; In:  a6 = the head object
; Out: g_SceneVars+8/+$A; re-enters the script (step coroutines, run the three children, yield)
; Evidence: after Op2_PlaceBodyBoxes / Tint_StepLine3 at $03B2A8; TAS f87285-f90392 (2670 frames); RAM f88320: head (160,99) = g_SceneVars+8/+$A $A0/$63
; ---------------------------------------------------------------------------
Boss_CatHead_PublishPos:
	move.w	$3E(a6),(g_SceneVars+$8).w
	move.w	$42(a6),(g_SceneVars+$A).w
	jsr	($7FE).w
