; ---------------------------------------------------------------------------
; Snd_SendSfx - driver API stub called by the V-int each frame: a0 = g_SndSfxSlots, jsr Snd_SendSfxBody (hands pending sound-effect requests to the Z80)
; In:  none (the V-int holds the Z80 bus already)
; Out: as Snd_SendSfxBody; a1 preserved
; Evidence: only caller VInt_Handler $77B8 between its own $A11100 bus request and release (full V-int path only, not VInt_BusyPath); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_SendSfx:
	move.l	a1,-(a7)
	lea.l	(g_SndSfxSlots).l,a0
	lea.l	(Snd_SendSfxBody).l,a1
	jsr	(a1)
	movea.l	(a7)+,a1
	rts
