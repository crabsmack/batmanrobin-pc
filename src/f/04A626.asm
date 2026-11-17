; ---------------------------------------------------------------------------
; Ending_FreezeCell - second ending scene: the story of Mr. Freeze in his Arkham cell (typewriter text on black, then his head and the barred moonlit window, the snow globe with the miniature city), plain scene init, font, the narrator object, then the unskippable loop
; In:  none
; Out: via Scene_LoopUnskippable
; Evidence: second call of Main_Ending; a6 script Scr2_04A572 (Lz_0FA352 picture, Lz_0FE6BE sprite bank at RAM frames $FC40); TextMap_LoadFont d0 = $E3AE; text Tbl_04A74A 'IN A FREEZING CELL IN ARKHAM ASYLUM...'; scenario-level-skip f15089, png w32 run f15200 (text), f15400 (Freeze), f16200 (globe)
; ---------------------------------------------------------------------------
Ending_FreezeCell:
	lea.l	Scr2_04A53C(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr2_04A572(pc),a6
	jsr	(VM6_Run).l
	move.w	#$E3AE,d0
	jsr	(TextMap_LoadFont).l
	bsr.w	Ending_SpawnCellNarrator
	bra.w	Scene_LoopUnskippable
