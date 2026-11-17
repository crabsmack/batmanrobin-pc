; ---------------------------------------------------------------------------
; Fx_RunScriptNoParent - runs the a4 spawn script at a4 on g_SpawnCtx with the scene's default sprite attribute
; In:  a4 = a4 script, d2-d5 = new objects' radius, heading, x, y
; Out: returns from the script's end op
; Evidence: ctx +$22 = g_SceneSprAttr then `bra VM4_Next` (leaves ctx +$36-$42 as they are); callers $13288, $1FECC, $2417E; 522 TAS frames
; ---------------------------------------------------------------------------
Fx_RunScriptNoParent:
	lea.l	(g_SpawnCtx).w,a6
	move.w	(g_SceneSprAttr).w,$22(a6)
	bra.w	VM4_Next
