; ---------------------------------------------------------------------------
; Scene_LoopSkippable - common loop of story, attract and stage-opening scenes: scroll mode 8, then per frame objects + camera; once g_SceneSkipDelay has run out and unless g_IntroNoSkip, Start newly pressed on pad 1 sets g_SceneResult 1 and fades out
; In:  none
; Out: ends through Scene_FadeOutExit (or a scene script's own end)
; Evidence: btst #7 of g_PadLive1+2 (newly pressed); callers $0484F6, $04885C, $0486B6, $0487B2, $048AD2, $048CBA, $0494BE, $04A180-$04A464 (bra.w)
; ---------------------------------------------------------------------------
Scene_LoopSkippable:
	move.w	#$8,(g_ScrollMode).w
; ---------------------------------------------------------------------------
; Scene_LoopSkippableKeepMode - Scene_LoopSkippable without setting the scroll mode (the scene keeps its own, e.g. mode $C)
; In:  none
; Out: as Scene_LoopSkippable
; Evidence: only caller: the story searchlight scene at $0485B6 after it sets g_ScrollMode = $C
; ---------------------------------------------------------------------------
Scene_LoopSkippableKeepMode:
	jsr	(Frame_Begin).l
	jsr	(Obj_RunList).l
	jsr	(Camera_UpdateView).l
	tst.b	(g_IntroNoSkip).w
	bne.s	loc_047CA8
	subq.w	#$1,(g_SceneSkipDelay).w
	bpl.s	loc_047CA8
	clr.w	(g_SceneSkipDelay).w
	btst	#$7,(g_PadLive1+2).l
	beq.s	loc_047CA8
	move.w	#$1,(g_SceneResult).w
	jsr	(Scene_FadeOutExit).l
loc_047CA8:
	jsr	(Frame_End).l
	bra.s	Scene_LoopSkippableKeepMode
; ---------------------------------------------------------------------------
; Scene_LoopUnskippable - scene loop without the Start skip: scroll mode 8, then objects + camera every frame until a script ends the scene
; In:  none
; Out: ends through the scene's script (Scene_EndAfterDelay etc.)
; Evidence: callers: the ending scenes ($048512, $04A5AA, $04A626), the continue screen $04916E, the title $0496A2
; ---------------------------------------------------------------------------
Scene_LoopUnskippable:
	move.w	#$8,(g_ScrollMode).w
; ---------------------------------------------------------------------------
; Scene_LoopUnskippableKeepMode - Scene_LoopUnskippable without setting the scroll mode: objects + camera every frame until a script ends the scene (the scene keeps its own mode, e.g. the player-select line scroll $30)
; In:  none
; Out: ends through the scene's script (Scene_EndAfterDelay etc.)
; Evidence: the loop of Scene_LoopUnskippable after its move.w #8,g_ScrollMode; only other caller PlayerSelect_Run (bra.w after it sets g_ScrollMode = $30)
; ---------------------------------------------------------------------------
Scene_LoopUnskippableKeepMode:
	jsr	(Frame_Begin).l
	jsr	(Obj_RunList).l
	jsr	(Camera_UpdateView).l
	jsr	(Frame_End).l
	bra.s	Scene_LoopUnskippableKeepMode
