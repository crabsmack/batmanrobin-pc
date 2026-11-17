; ---------------------------------------------------------------------------
; Tint_FlashC1 - starts a line-tint flash: g_TintLevel +$C decaying to 0 by 2 every frame
; In:  none
; Out: g_TintTarget = 0, g_TintLevel = $C, g_TintDelay = g_TintDelayCount = 0
; Evidence: the brightness reaches a palette line through Tint_StepLine0/1/3 (line 3 in the TAS: RAM f5040 $F4D3-$F4D5 = $0A, f5100 $08 while a boss is hit, png f005400); pointers in boss/stage scripts ($018144, $0181A0, $018232, $01C612, $025CD2, $028E10 ...: a2 native calls); 20 corpus runs, TAS 1004 frames
; ---------------------------------------------------------------------------
Tint_FlashC1:
	clr.b	(g_TintTarget).w
	move.b	#$C,(g_TintLevel).w
	move.b	#$0,(g_TintDelay).w
	move.b	#$0,(g_TintDelayCount).w
	rts
; ---------------------------------------------------------------------------
; Tint_FlashC3 - starts a line-tint flash: g_TintLevel +$C decaying to 0 by 2 every 3rd frame
; In:  none
; Out: g_TintTarget = 0, g_TintLevel = $C, g_TintDelay = g_TintDelayCount = 2
; Evidence: same layout as Tint_FlashC1; jsr from the native $029608 after Fade_FlashWhite7; not run in the corpus
; ---------------------------------------------------------------------------
Tint_FlashC3:
	clr.b	(g_TintTarget).w
	move.b	#$C,(g_TintLevel).w
	move.b	#$2,(g_TintDelay).w
	move.b	#$2,(g_TintDelayCount).w
	rts
; ---------------------------------------------------------------------------
; Tint_FlashE2 - starts a line-tint flash: g_TintLevel +$E decaying to 0 by 2 every 2nd frame
; In:  none
; Out: g_TintTarget = 0, g_TintLevel = $E, g_TintDelay = g_TintDelayCount = 1
; Evidence: same layout as Tint_FlashC1; pointers at $048E14 and $048E50 (scene code next to the tint users $48C36/$48E80); not run in the corpus
; ---------------------------------------------------------------------------
Tint_FlashE2:
	clr.b	(g_TintTarget).w
	move.b	#$E,(g_TintLevel).w
	move.b	#$1,(g_TintDelay).w
	move.b	#$1,(g_TintDelayCount).w
	rts
; ---------------------------------------------------------------------------
; Tint_FlashA2 - starts a line-tint flash: g_TintLevel +$A decaying to 0 by 2 every 2nd frame
; In:  none
; Out: g_TintTarget = 0, g_TintLevel = $A, g_TintDelay = g_TintDelayCount = 1
; Evidence: same layout as Tint_FlashC1; pointers at $02D084, $048B76, $048DC6, $048DF6, $048E32; not run in the corpus
; ---------------------------------------------------------------------------
Tint_FlashA2:
	clr.b	(g_TintTarget).w
	move.b	#$A,(g_TintLevel).w
	move.b	#$1,(g_TintDelay).w
	move.b	#$1,(g_TintDelayCount).w
	rts
; ---------------------------------------------------------------------------
; Tint_FlashE6 - starts a line-tint flash: g_TintLevel +$E decaying to 0 by 2 every 6th frame
; In:  none
; Out: g_TintTarget = 0, g_TintLevel = $E, g_TintDelay = g_TintDelayCount = 5
; Evidence: same layout as Tint_FlashC1; pointer at $048B82; not run in the corpus
; ---------------------------------------------------------------------------
Tint_FlashE6:
	clr.b	(g_TintTarget).w
	move.b	#$E,(g_TintLevel).w
	move.b	#$5,(g_TintDelay).w
	move.b	#$5,(g_TintDelayCount).w
	rts
; ---------------------------------------------------------------------------
; Tint_StepLine0 - steps the line tint and applies it to palette line 0 (equal red, green and blue brightness offsets)
; In:  g_Tint*
; Out: d0 = g_TintLevel (Z set when 0), g_Pal0TintR/G/B = d0; d1 trashed
; Evidence: Tint_Step then three byte stores; caller $049AD4 when g_P1IsRobin is clear (Tint_StepLine1 otherwise); not run in the corpus
; ---------------------------------------------------------------------------
Tint_StepLine0:
	bsr.w	Tint_Step
	move.b	d0,(g_Pal0TintR).w
	move.b	d0,(g_Pal0TintG).w
	move.b	d0,(g_Pal0TintB).w
	rts
; ---------------------------------------------------------------------------
; Tint_StepLine1 - steps the line tint and applies it to palette line 1 (equal red, green and blue brightness offsets)
; In:  g_Tint*
; Out: d0 = g_TintLevel (Z set when 0), g_Pal1TintR/G/B = d0; d1 trashed
; Evidence: Tint_Step then three byte stores; callers $049AD4 (when g_P1IsRobin), pointers at $02962A, $03C752; not run in the corpus
; ---------------------------------------------------------------------------
Tint_StepLine1:
	bsr.w	Tint_Step
	move.b	d0,(g_Pal1TintR).w
	move.b	d0,(g_Pal1TintG).w
	move.b	d0,(g_Pal1TintB).w
	rts
