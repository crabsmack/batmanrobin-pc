; ---------------------------------------------------------------------------
; Boss_Claw_Spawn - warehouse trigger callback: spawns Harley's giant crane claw (8 pool blocks: body, chain and three finger blocks) running Scr2_01E778, with its palette in line 3
; In:  d4/d5 = x/y (trigger entry Tbl_016632 +$A2)
; Out: d0 = 0, or Z clear when the pool is short
; Evidence: Obj_Spawn VM2_RunObject d7 = 8, ObjPool_Clear on $E(a6), Pal_LoadLine3 Tbl_01E748, g_Rot3DInner angle 0 = $2F8, clears the Harley-beaten flag g_SceneVars; TAS f10511; shots f11460/f12420 (the ball-jointed claw on its chain, the crane tower in the background)
; ---------------------------------------------------------------------------
Boss_Claw_Spawn:
	lea.l	(VM2_RunObject).l,a0
	moveq	#$8,d7
	jsr	(Obj_Spawn).l
	bne.w	loc_01E746
	move.l	#$1E778,$2A(a4)
	movea.w	$E(a6),a0
	jsr	(ObjPool_Clear).l
	lea.l	(Tbl_01E748).l,a0
	jsr	(Pal_LoadLine3).l
	move.w	#$2F8,(g_Rot3DInner).w
	clr.b	(g_SceneVars).w
	moveq	#$0,d0
loc_01E746:
	rts
