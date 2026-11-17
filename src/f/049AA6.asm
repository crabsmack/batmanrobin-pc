; ---------------------------------------------------------------------------
; PlayerSelect_Run - scene routine of the player-select screen: plain scene with 128x32 planes (Scr6_049A56), the graphics (Scr6_049A68: two LZ blocks with maps drawn on planes A and B, palettes), Batman chosen first, the chooser and the PLAYER / SELECT captions, plane A line scroll (mode $30), then the scene loop $47CB6
; In:  -
; Out: g_P1IsRobin = the choice; returns when the chooser ends the scene
; Evidence: only caller Main_NewGame; sf.b g_P1IsRobin; g_ScrollMode = $30; TAS f412, scenario-robin f466; png x-robin f540: Batman in the top half, Robin's R in the bottom half, PLAYER / SELECT down the sides
; ---------------------------------------------------------------------------
PlayerSelect_Run:
	lea.l	Scr6_049A56(pc),a6
	jsr	(Scene_InitPlain).l
	lea.l	Scr6_049A68(pc),a6
	jsr	(VM6_Run).l
	sf.b	(g_P1IsRobin).w
	jsr	(PlayerSelect_SpawnChooser).l
	jsr	(PlayerSelect_SpawnCaption).l
	move.w	#$30,(g_ScrollMode).w
	bra.w	Scene_LoopUnskippableKeepMode
