; ---------------------------------------------------------------------------
; Mot_JumpGravity - reads a height word and returns the gravity of a jump that rises that many pixels in the time word that follows (Mot_JumpGravityD1)
; In:  a2 = script: word height, word time; a4 = speed block
; Out: d1 = gravity (16.16 per run, positive for a positive height); a2 past both words
; Evidence: moveq/move.w (a2)+,d1 then falls into $1D82; callers Op2_ArcX, Op2_SetJumpGravity, Op2_JumpAnim, Op2_Jump, Op2_FallAnim
; ---------------------------------------------------------------------------
Mot_JumpGravity:
	moveq	#$0,d1
	move.w	(a2)+,d1
