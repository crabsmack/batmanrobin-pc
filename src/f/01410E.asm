; ---------------------------------------------------------------------------
; Text_DrawAtObject - a2 native: draws the string in the following dc.l as sprite letters at the object's position
; In:  a6 = object (+$3E/+$42 x/y), a5 = sprite block (+$38 attr), a2 = script at the dc.l string
; Out: a2 past the long; a1-a6 preserved
; Evidence: op $2CBC pointers at $49668 and $4A29C-$4A51E (stage cards) to the strings at $1407A-$1410D (SELECT, PLAYER, GAME OVER, LEVEL ONE ... SNOW IN JULY?); ran in game-over, level-skip, robin and two-player scenarios
; ---------------------------------------------------------------------------
Text_DrawAtObject:
	move.w	$3E(a6),d4
	move.w	$42(a6),d5
	movea.l	(a2)+,a0
	movem.l	a1-a6,-(a7)
	movea.l	a0,a2
	move.w	$38(a5),d6
	jsr	(Text_DrawSprites).l
	movem.l	(a7)+,a1-a6
	rts
