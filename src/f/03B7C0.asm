; ---------------------------------------------------------------------------
; Boss_CatHead_EyeShotAt - the eye-shot natives' middle: picks the shot script $03B812 and goes on into Boss_CatHead_EyeFxAt
; In:  d0.w = the eye's x offset; as Boss_CatHead_EyeSparkLeft
; Out: a0 = Scr4_03B812; then Boss_CatHead_EyeFxAt's
; Evidence: lea Scr4_03B812,a0, then the next file; the bra.s of Boss_CatHead_EyeShotLeft and the fall-through of Boss_CatHead_EyeShotRight; no corpus run reaches it (raw-stubs 2026-10-30)
; ---------------------------------------------------------------------------
Boss_CatHead_EyeShotAt:
	lea.l	(Scr4_03B812).l,a0
