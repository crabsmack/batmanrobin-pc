; ---------------------------------------------------------------------------
; Attract_SkyscraperScroll - native of the skyscraper camera: g_CameraY = its y, plane A layer scrolled to it (Layer_ScrollStep, Scroll_SetPlaneA), plane B V-scroll = camera y / 2, H-scroll B 0; the line tint (Tint_Step) brightens line 3 and darkens lines 0-2 by the same amount (lightning)
; In:  a6 = camera object
; Out: g_CameraY, g_LayerA, scroll words, g_Pal0-3Tint
; Evidence: only caller Op2_CallNativeRelSaved at $048B46 in Scr2_048B2A; neg.b between the line-3 and lines-0-2 tints; idle 391-926 + 3925n, scenario-ending 138818-143260
; ---------------------------------------------------------------------------
Attract_SkyscraperScroll:
	move.w	$42(a6),d0
	move.w	d0,(g_CameraY).w
	lea.l	(g_LayerA).l,a6
	sub.w	$2(a6),d0
	move.w	d0,$6(a6)
	jsr	(Layer_ScrollStep).l
	lea.l	(g_LayerA).l,a6
	jsr	(Scroll_SetPlaneA).l
	move.w	(g_CameraY).w,d0
	asr.w	#$1,d0
	move.w	d0,(g_VScrollB).w
	clr.w	(g_HScrollB).w
	jsr	(Tint_Step).l
	move.b	d0,(g_Pal3TintR).w
	move.b	d0,(g_Pal3TintG).w
	move.b	d0,(g_Pal3TintB).w
	neg.b	d0
	move.b	d0,(g_Pal0TintR).w
	move.b	d0,(g_Pal0TintG).w
	move.b	d0,(g_Pal0TintB).w
	move.b	d0,(g_Pal1TintR).w
	move.b	d0,(g_Pal1TintG).w
	move.b	d0,(g_Pal1TintB).w
	move.b	d0,(g_Pal2TintR).w
	move.b	d0,(g_Pal2TintG).w
	move.b	d0,(g_Pal2TintB).w
	rts
; ---------------------------------------------------------------------------
; Attract_PlaneBFollowObj - native of the skyscraper's second object: plane B V-scroll = g_CameraY - its y, H-scroll B = its x (plane B's picture moves with the object)
; In:  a6 = object
; Out: g_VScrollB, g_HScrollB
; Evidence: only caller Op2_CallNativeRelSaved at $048BCE in Scr2_048B9A; runtime entry in the ctarget union
; ---------------------------------------------------------------------------
Attract_PlaneBFollowObj:
	move.w	(g_CameraY).w,d0
	sub.w	$42(a6),d0
	move.w	d0,(g_VScrollB).w
	move.w	$3E(a6),(g_HScrollB).w
	rts
; ---------------------------------------------------------------------------
; Attract_Rooftop - fourth attract scene: Batman and Robin standing on a rooftop skylight under the red sky; plain scene init + art, their object, skippable loop (then the title)
; In:  none
; Out: via Scene_LoopSkippable
; Evidence: Tbl_00A152 entry 4 ($048CBA, inside the old $048C36 file); Scr6_0489F4/048A86; idle png f9000/f16800 in the windows 8776-9010 / 16626-16860
; ---------------------------------------------------------------------------
Attract_Rooftop:
	lea.l	Scr6_0489F4(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_048A86(pc),a6
	jsr	(VM6_Run).l
	bsr.w	Attract_SpawnRooftopHeroes
	bra.w	Scene_LoopSkippable
