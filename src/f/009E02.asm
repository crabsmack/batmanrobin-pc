; ---------------------------------------------------------------------------
; Main_Start - the game's main program after boot: clears RAM and VRAM/CRAM, starts the sound driver, seeds the RNG, sets the new-game defaults, then loops forever through attract, title, play, continue/game over and ending
; In:  none (entered by jmp from Boot_InitIOPorts)
; Out: never returns
; Evidence: clears $FF0000-$FFFFFF, $ADA6 CRAM, $AD84 VRAM, $1E5FFA (in the sound driver: its init, inferred), RNG $F5FC/$F5FE/$F600 = $79/$4B/$D3, VDP regs from Tbl_0155BC; defaults: $F60E 0, $F610 1, lives $F618 2, g_StartCredits 6, weapon $F614/$F616 0, $F61E/$F620 $100 (enemy/boss energy scale x1.0, read via $F64A/$F64C by the a2 op $39AC); TAS f3, once
; ---------------------------------------------------------------------------
Main_Start:
	ori.w	#$700,sr
	move.w	#$8124,($C00004).l
	lea.l	(g_SceneBuf&$FFFFFF).l,a0
	moveq	#$0,d0
	move.w	#$3FFF,d7
loc_009E1A:
	move.l	d0,(a0)+
	dbra	d7,loc_009E1A
	bsr.w	VDP_ClearCRAM
	bsr.w	VDP_ClearVRAM
	jsr	(Snd_Init).l
	move.w	#$79,(g_RandState).w
	move.w	#$4B,(g_RandState2).w
	move.w	#$D3,($FFFFF600).w
	lea.l	(Tbl_0155BC).l,a0
	jsr	(VDP_LoadRegs).l
	addq.b	#$1,(g_VIntOff).w
	move.w	#$8164,($C00004).l
	andi.w	#$F8FF,sr
	move.w	#$0,(g_DebugPause).w
	move.w	#$1,(g_UnkStart_F610).w
	move.w	#$2,(g_StartLives).w
	move.w	#$6,(g_StartCredits).w
	move.w	#$0,(g_StartWeaponType).w
	move.w	#$0,(g_StartWeaponLvl).w
	move.w	#$1,(g_UnkStart_F61A).w
	move.w	#$1,(g_UnkStart_F61C).w
	move.w	#$100,(g_StartEnemyScale).w
	move.w	#$100,(g_StartBossScale).w
	st.b	(g_IntroNoSkip).w
; ---------------------------------------------------------------------------
; Main_Attract - the power-on / attract sequence: title music, music changes locked, runs the scene list Tbl_00A152 ($49DA6, $49F62, $48AD2, $48CBA) with the intro not skippable on the first pass
; In:  none
; Out: falls into Main_Title
; Evidence: Sound_PlayMusic($11), st $F6FE (Sound_PlayMusic then ignores requests), $F701 set around the list ($47C6A ignores Start while it is set; $48AD2 clears it); $49DA6 runs at TAS f10
; ---------------------------------------------------------------------------
Main_Attract:
	move.w	#$11,d0
	bsr.w	Sound_PlayMusic
	st.b	(g_MusicLock).w
	lea.l	(Tbl_00A152).l,a0
	jsr	(Scene_RunList).l
	sf.b	(g_IntroNoSkip).w
; ---------------------------------------------------------------------------
; Main_Title - shows the title ($496A2); Start begins a new game, otherwise the attract story list Tbl_00A136 runs and the loop returns to the title (Start) or the full attract
; In:  none
; Out: goes to Main_NewGame when g_SceneResult = 1 after the title
; Evidence: Tbl_00A136 = $484F6, $4885C, $485B6, $486B6, $487B2, $49E06 (ran in the idle run, not in the TAS); $496A2 at TAS f392
; ---------------------------------------------------------------------------
Main_Title:
	move.w	#$11,d0
	bsr.w	Sound_PlayMusic
	st.b	(g_MusicLock).w
	jsr	(Title_Run).l
	cmpi.w	#$1,(g_SceneResult).w
	beq.s	Main_NewGame
	lea.l	(Tbl_00A136).l,a0
	jsr	(Scene_RunList).l
	cmpi.w	#$1,(g_SceneResult).w
	beq.s	Main_Title
	bra.s	Main_Attract
; ---------------------------------------------------------------------------
; Main_NewGame - player select ($49AA6), then unlocks music, clears g_StartPad and g_SectionId and sets up the players (Player_NewGameSetup)
; In:  none
; Out: falls into Main_PlayLoop
; Evidence: sf $F6FE, sf g_StartPad, clr.w $F602, jsr Player_NewGameSetup; reached when the title ends with result 1
; ---------------------------------------------------------------------------
Main_NewGame:
	jsr	(PlayerSelect_Run).l
	sf.b	(g_MusicLock).w
	sf.b	(g_StartPad).w
	clr.w	(g_SectionId).w
	jsr	(Player_NewGameSetup).l
; ---------------------------------------------------------------------------
; Main_PlayLoop - the play loop: advances to the next section in the normal order table Tbl_009FFC and runs it, until the order ends (ending) or a section ends in game over
; In:  g_SectionId = the section just played (0 at a new game)
; Out: game over -> continue screen ($4916E, music $13): continue restarts the section through Tbl_00A05C, else Main_NoContinue
; Evidence: Tbl_009FFC = (from,to) word pairs 0000->0001->1100->1300->1301->1400->0002->2100 ... 4400->4401, long 0 ends; Tbl_00A05C maps each section to itself (retry after continue); result 2 = SceneCb_CheckGameOver
; ---------------------------------------------------------------------------
Main_PlayLoop:
	lea.l	(Tbl_009FFC).l,a0
; ---------------------------------------------------------------------------
; Main_RunSection - runs the next section from the order table in a0 (Section_RunNext) and dispatches on the outcome: order ended -> Main_Ending, result 2 -> continue screen, else the next section
; In:  a0 = order table
; Out: loops
; Evidence: bsr Section_RunNext / beq Main_Ending / cmpi.w #2,$F608; after a continue a0 = Tbl_00A05C so the failed section restarts
; ---------------------------------------------------------------------------
Main_RunSection:
	bsr.w	Section_RunNext
	beq.w	Main_Ending
	cmpi.w	#$2,(g_SceneResult).w
	bne.s	Main_PlayLoop
	move.w	#$13,d0
	bsr.w	Sound_PlayMusic
	move.w	(g_SectionId).w,-(a7)
	jsr	(Continue_Run).l
	move.w	(a7)+,(g_SectionId).w
	cmpi.w	#$1,(g_SceneResult).w
	bne.s	Main_NoContinue
	jsr	(Player_InitRecords).l
	lea.l	(Tbl_00A05C).l,a0
	bra.s	Main_RunSection
; ---------------------------------------------------------------------------
; Main_NoContinue - after the continue screen was declined: game over screen ($494BE), then the title (result 1) or the attract sequence
; In:  none
; Out: Main_Title or Main_Attract
; Evidence: $494BE ran only in scenario-game-over, x-die, x-die2; $4916E ran in those and scenario-continue
; ---------------------------------------------------------------------------
Main_NoContinue:
	jsr	(GameOver_Run).l
	cmpi.w	#$1,(g_SceneResult).w
	beq.w	Main_Title
	bra.w	Main_Attract
; ---------------------------------------------------------------------------
; Main_Ending - after the last section: ending music $12 and the ending scenes ($48512, $4A626, $4A5AA), then back to the attract sequence
; In:  none
; Out: Main_Attract
; Evidence: reached when Section_RunNext finds no entry for section $4401; $48512 ran in scenario-ending, scenario-level-skip, x-ending
; ---------------------------------------------------------------------------
Main_Ending:
	move.w	#$12,d0
	bsr.w	Sound_PlayMusic
	jsr	(Ending_AsylumGate).l
	jsr	(Ending_FreezeCell).l
	jsr	(Ending_Epilogue).l
	bra.w	Main_Attract
