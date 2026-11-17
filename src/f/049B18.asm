; ---------------------------------------------------------------------------
; PlayerSelect_SpawnChooser - spawns the player-select chooser Scr2_049B24: positions both halves (x -255, y -280), reads the pad, slides the halves (PlayerSelect_SlideBatmanHalf/RobinHalf) and scrolls them; A/B/C/Up/Down toggle the hero and restart the 300-frame timer; Start or the timer running out confirms: 4 tint flashes, then the scene ends after 10 frames
; In:  -
; Out: a6 = the object (Obj_SpawnEnemy)
; Evidence: Scr2_049B24: Op2_PlaceOnScreen $FF01/$FEE8, Op2_SetTimer0 $12C0, natives $49B04/$49BA0/$49BEE/$49AD4, $49B6C toggles via $49B72, $49B7C: Op2_SetCounter0 4 x Tint_FlashC1, Scene_EndAfterDelay 0/$A; TAS f431
; ---------------------------------------------------------------------------
PlayerSelect_SpawnChooser:
	lea.l	(Scr2_049B24).l,a2
	jmp	(Obj_SpawnEnemy).l
