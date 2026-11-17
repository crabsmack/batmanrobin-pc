; ---------------------------------------------------------------------------
; Op4_KillOffScreen - a4 op: deletes the object when its drawn position d4/d5 is outside the on-screen window (x - g_OnScreenLeft >= $170 or y - g_OnScreenTop >= $110, unsigned); otherwise continues
; In:  a4 = script (no operand); d4/d5 = drawn x/y words (from the motion op)
; Out: Op4_Kill when off screen, else NEXT
; Evidence: cmpi #$170 / #$110 with bcc Op4_Kill (the same test as the draw op's tail at $67CA); script use $26D6A after $61FC; 3 corpus runs (fuzz only)
; ---------------------------------------------------------------------------
Op4_KillOffScreen:
	move.w	d4,d0
	sub.w	(g_OnScreenLeft).w,d0
	cmpi.w	#$170,d0
	bcc.w	Op4_Kill
	move.w	d5,d0
	sub.w	(g_OnScreenTop).w,d0
	cmpi.w	#$110,d0
	bcc.w	Op4_Kill
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_ReplaceSelf - a4 op: turns the object into a new one in place: drops its collision box, restarts its motion from d2-d5 (plus an optional parameter table) with a new script and runs that script now
; In:  a4 = script: word (ignored here: the spawn ops' failure offset), word script offset, word parameter-table offset (0 = none); d2-d5 = radius, heading, x, y (16.16)
; Out: VM4_ResumeA5 (NEXT in the new script)
; Evidence: pushes $55C2 and enters the spawn init $6156: Hitbox_Unlink + ObjPool_Free on $14(a6), $8(a6) = new script, $26-$54 from d2-d5 / the table; script uses $14C3E, $151D4 (after $63A2 loads the own position); 147 corpus runs
; ---------------------------------------------------------------------------
Op4_ReplaceSelf:
	movea.l	a6,a5
	pea.l	(VM4_ResumeA5).l
	bra.w	VM4_ReinitSelf
