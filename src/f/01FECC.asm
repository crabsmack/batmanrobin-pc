; ---------------------------------------------------------------------------
; Stage1_RunTruck - section $1400: the stage 1 parade truck: music $D, a6 scene script Scr6_01FDF6, plane A placed with Layer_MoveYWithCamera, trigger table Tbl_0166EA, plane scroll mode 8, health refill, then a frame loop with animated tiles, Scroll_SetPlaneA and plane B streaming left 16 px a frame
; In:  -
; Out: never returns normally
; Evidence: Tbl_00A0AC maps $1400 here; subi.w #$10,g_HScrollB each frame; Layer_AnimateTiles; TAS f13217 (shots f13860, f18000: flatbed truck, balloon bombs; the Joker balloon at the end)
; ---------------------------------------------------------------------------
Stage1_RunTruck:
	move.w	#$D,d0
	jsr	(Sound_PlayMusic).l
	lea.l	Scr6_01FDF6(pc),a6
	jsr	(Scene_InitStage).l
	lea.l	(Tbl_015634).l,a0
	jsr	(Pal_LoadLine0).l
	lea.l	Tbl_01FA36(pc),a0
	jsr	(Pal_LoadLine3).l
	lea.l	Scr6_01FE32(pc),a6
	jsr	(VM6_Run).l
	lea.l	(g_LayerA).l,a6
	move.w	#$8,d0
	jsr	(Layer_MoveYWithCamera).l
	jsr	(Layer_DrawScreen).l
	lea.l	(Layer_FlushTileQueueIfIdle).l,a0
	jsr	(VIntObj_New).l
	lea.l	(Tbl_0166EA).l,a0
	jsr	(Trigger_LoadX).l
	move.w	#$8,(g_ScrollMode).w
	move.w	#$100,(g_Player1+$4).w
	move.w	#$100,(g_Player2+$4).w
loc_01FF40:
	jsr	(Frame_BeginStage).l
	jsr	(Frame_RunWorldLayerA).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_AnimateTiles).l
	lea.l	(g_LayerA).l,a6
	jsr	(Layer_ScrollStep).l
	jsr	(Scroll_SetPlaneA).l
	subi.w	#$10,(g_HScrollB).w
	jsr	(Frame_End).l
	bra.s	loc_01FF40
; ---------------------------------------------------------------------------
; unk_Stage1_01FF78 - street trigger callback: starts the a4 object Scr4_01FF8C at the trigger x/y; it shows RAM-bank frame $FC40 (4 pieces, 64 px wide, from 31 px above) until it leaves the screen; its role is not seen
; In:  d4/d5 = trigger x/y (entries at x $801 and $8A9, y $70 in Tbl_015F5A)
; Out: d0 = 0
; Evidence: Fx_RunScriptNoParent; a4 objects at (2049,112) and (2217,112) = the two bank-sign hanging points in RAM f1680-f1980; frame table entry $A3A4 (art handle $8002, pieces $FF2374); no distinct sprite visible there in the f1740 shot (crop), so what it draws is unknown
; ---------------------------------------------------------------------------
unk_Stage1_01FF78:
	swap	d4
	swap	d5
	lea.l	(Scr4_01FF8C).l,a4
	jsr	(Fx_RunScriptNoParent).l
	moveq	#$0,d0
	rts
