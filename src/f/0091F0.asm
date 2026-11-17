; ---------------------------------------------------------------------------
; Fx_RunScriptArg - runs the a4 spawn script given by the script's next dc.l (Fx_RunScript)
; In:  a2 = script at the dc.l, d2-d5 as Fx_RunScript
; Out: a2 past the dc.l
; Evidence: falls into Fx_RunScript; tail of the Fx_SpawnAt* callbacks; $1A04C, $25E4C, $29534
; ---------------------------------------------------------------------------
Fx_RunScriptArg:
	movea.l	(a2)+,a0
