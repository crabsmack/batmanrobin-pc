; ---------------------------------------------------------------------------
; Options_Init - opens the options page from the title menu: blanks the text map, puts the cursor on CREDITS and draws the static texts (OPTIONS, CREDITS:, MUSIC TEST:, SOUND TEST:, PRESS START TO EXIT) from Tbl_0498EA
; In:  -
; Out: g_MenuCursor = 0
; Evidence: Op2_CallNativeRel from $49756 (Op2_BranchVar1Eq 1 = OPTIONS chosen); text bytes of Tbl_0498EA; ran only in scenario-options (f468); png x-options f600
; ---------------------------------------------------------------------------
Options_Init:
	jsr	(TextMap_Clear).l
	clr.w	(g_MenuCursor).w
	lea.l	Tbl_0498EA(pc),a0
	jmp	(TextMap_Run).l
; ---------------------------------------------------------------------------
; Options_Step - options page per-frame native: Up/Down/Left/Right edit the settings of Tbl_049994 (credits g_StartCredits 0-2, music test 0-$F, sound test 0-$39), the values Tbl_049948 is drawn and sent, a held button restarts the $7080-frame idle timeout, and A/B/C plays the music test song or the sound test effect (value + 1)
; In:  -
; Out: g_StartCredits, g_OptMusicTest, g_OptSoundTest, g_MenuCursor; g_SceneTimer = $7080 when a button is held
; Evidence: Menu_Update Tbl_049994 = ($F612 max 2, $F60A max $F, $F60C max $39); g_Pad1+2 & $70 with cursor 1 -> Sound_RestartMusic, cursor 2 -> Snd_PlaySfx (the Op2_PlaySound driver call); Op2_CallNativeRel from $49760; runtime entry in scenario-options / x-options (f468-f1449)
; ---------------------------------------------------------------------------
Options_Step:
	lea.l	Tbl_049994(pc),a0
	jsr	(Menu_Update).l
	lea.l	Tbl_049948(pc),a0
	jsr	(TextMap_Run).l
	jsr	(TextMap_Upload).l
	tst.b	(g_Pad1).w
	beq.s	loc_049A22
	move.w	#$7080,(g_SceneTimer).w
loc_049A22:
	move.b	(g_Pad1+2).w,d0
	andi.b	#$70,d0
	beq.s	loc_049A3C
	move.w	(g_MenuCursor).w,d0
	cmpi.w	#$1,d0
	beq.s	loc_049A3E
	cmpi.w	#$2,d0
	beq.s	loc_049A4A
loc_049A3C:
	rts
loc_049A3E:
	move.w	(g_OptMusicTest).w,d0
	addq.w	#$1,d0
	jmp	(Sound_RestartMusic).l
loc_049A4A:
	move.w	(g_OptSoundTest).w,d0
	addq.w	#$1,d0
	jmp	(Snd_PlaySfx).l
