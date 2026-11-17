; ---------------------------------------------------------------------------
; TextMap_LoadFont - loads the menu font: DMAs its 132 tiles from $58AA2 to VRAM tile d0, sets the font map and tile base for TextMap_Run, and loads Tbl_015634 into palette line 3
; In:  d0 = tile base word (priority/palette bits + tile index, e.g. $E1C9)
; Out: g_TextMapTileBase = d0, g_TextMapFont = Tbl_0586F2
; Evidence: VDP regs $93/$94 = $0840 words, $95-$97 = $02C551 (word address of $58AA2), destination (d0 & $7FF) * 32, display off ($8134) during it, Pal_LoadLine3; callers the menu/text scene inits ($4916E, $496A2, $49F62, $4A5AA, $4A626); TAS f164, f404
; ---------------------------------------------------------------------------
TextMap_LoadFont:
	move.w	d0,(g_TextMapTileBase).w
	move.l	#$586F2,(g_TextMapFont).w
	lea.l	($C00004).l,a0
	move.w	#$8134,(a0)
	move.l	#$93409408,(a0)
	move.l	#$955196C5,(a0)
	move.w	#$9702,(a0)
	andi.l	#$7FF,d0
	lsl.l	#$7,d0
	lsr.w	#$2,d0
	swap	d0
	ori.l	#$40000080,d0
	move.l	d0,(g_VDPCmdBuf).w
	move.w	(g_VDPCmdBuf).w,(a0)
	move.w	(g_VDPCmdBuf+2).w,(a0)
	move.w	#$8164,(a0)
	lea.l	(Tbl_015634).l,a0
	jmp	(Pal_LoadLine3).l
