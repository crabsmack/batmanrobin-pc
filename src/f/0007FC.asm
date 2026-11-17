; ---------------------------------------------------------------------------
; Op2_Native - a2 op: leaves the script and runs the 68000 code that follows it (jmp (a2)); that code returns to script with `jsr VM2_Enter`, which resumes right after the jsr
; In:  a2 = address of the native code (right after this op word)
; Out: whatever the native fragment does; a2/a3-a6 as in the script
; Evidence: the file also holds VM2_Enter ($7FE: movea.l (a7)+,a2 / NEXT, 73 `jsr $7FE.w` sites, dkit.toml noreturn) and the core ops $804-$95C below; e.g. $13F36, $1806C, $197FC are native fragments that end in jsr $7FE.w; 12k TAS frames
; ---------------------------------------------------------------------------
Op2_Native:
	jmp	(a2)
; ---------------------------------------------------------------------------
; VM2_Enter - enters a2 script from native code: pops the return address of jsr $7FE.w into a2 and dispatches (NEXT), so the script is the bytes after the jsr
; In:  (sp) = return address = script; a3-a6 = the running object (the native fragment runs under VM2_RunObject)
; Out: NEXT; never returns to the jsr
; Evidence: 73 jsr $7FE.w sites (e.g. $13F36, $1806C, $197FC end native fragments with it); dkit.toml [disasm] noreturn
; ---------------------------------------------------------------------------
VM2_Enter:
	movea.l	(a7)+,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetOwnerId - a2 op: sets the object's owner/side id $58(a6) from a word operand
; In:  a2 = script: word id (other writers use 1/2 for players, $A/$B for enemies)
; Out: NEXT
; Evidence: $385A/$3798 copy $58(a6) into each collision box +$10; $102D6 picks pad 1 or 2 by $58(a6) = 1; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetOwnerId:
	move.w	(a2)+,$58(a6)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetResume - a2 op: stores operand address + word offset in $2A(a4), the point VM2_RunObject resumes at next frame, and continues here
; In:  a2 = script: word offset
; Out: NEXT
; Evidence: $2A(a4) is what VM2_RunObject loads into a2; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetResume:
	movea.l	a2,a0
	adda.w	(a2)+,a0
	move.l	a0,$2A(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SetResumeHere - a2 op: stores the address of the next op in $2A(a4) as the resume point
; In:  a2 = script (no operand)
; Out: NEXT
; Evidence: move.l a2,$2A(a4) then NEXT; executed in the TAS
; ---------------------------------------------------------------------------
Op2_SetResumeHere:
	move.l	a2,$2A(a4)
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_YieldGoto - a2 op: ends this frame's run; next frame resumes at operand address + word offset
; In:  a2 = script: word offset
; Out: falls into Op2_Yield (d0 = 0 from VM2_RunObject)
; Evidence: adda.w (a2),a2 then the Op2_Yield code; executed in the TAS
; ---------------------------------------------------------------------------
Op2_YieldGoto:
	adda.w	(a2),a2
; ---------------------------------------------------------------------------
; Op2_Yield - a2 op: ends this frame's run: saves the next op's address in $2A(a4), restores sp from g_VM2SavedSP and returns d0 = 0 from VM2_RunObject
; In:  a2 = script (no operand)
; Out: rts to VM2_RunObject's caller with d0 = 0
; Evidence: native callers branch here to wait a frame (e.g. $120E retries a failed pool allocation with a2 backed up; $2486, $3B76, $437A, $43E2); executed in the TAS
; ---------------------------------------------------------------------------
Op2_Yield:
	move.l	a2,$2A(a4)
; ---------------------------------------------------------------------------
; Op2_YieldAgain - a2 op: ends this frame's run without moving the resume point, so the next frame runs the script again from the point saved last: the saved stack pointer, then VM2_RunObject's caller's return with d0 = 0
; In:  the VM block's saved resume point (unchanged)
; Out: rts to VM2_RunObject's caller with d0 = 0
; Evidence: the Op2_Yield code after its move.l a2,$2A(a4); 786,961 computed jumps in the corpus (call-site log: NEXT $7E2, $2CBA, $2D26, $2D2E, $3A52 ...)
; ---------------------------------------------------------------------------
Op2_YieldAgain:
	movea.l	(g_VM2SavedSP).w,a7
	moveq	#$0,d0
; ---------------------------------------------------------------------------
; Native_NoOp - a native that does nothing: the rts closing Op2_Yield, stored as a handler wherever one must be callable but has nothing to do
; In:  none
; Out: none
; Evidence: jsr/jmp (a1) to it from Hitbox_Strike $4E62, Hitbox_StrikeAll $4E74 and $37574 (call-site log: 4448, 9601, 2480 calls); NOTES: never-run ops set a coroutine slot to it; a terrain routine
; ---------------------------------------------------------------------------
Native_NoOp:
	rts
; ---------------------------------------------------------------------------
; Op2_BranchOffScrBox - a2 op: branches when the object's screen position ($3E(a6)-g_CameraX, $42(a6)-g_CameraY) is outside the box [xlo,xhi] x [ylo,yhi]
; In:  a2 = script: words xlo, xhi, ylo, yhi, offset
; Out: branch by offset, else NEXT after the five words
; Evidence: x part here, y part shared with Op2_BranchOffScrY ($882); executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchOffScrBox:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_000846
	cmp.w	(a2)+,d0
	ble.s	Op2_BranchOffScrY
	addq.l	#$4,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_000846:
	addq.l	#$6,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchOffScrX - a2 op: branches when the object's screen x ($3E(a6) - g_CameraX) is outside [lo,hi]
; In:  a2 = script: words lo, hi, offset
; Out: branch by offset (via $2D20/$2D22), else NEXT after the offset
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchOffScrX:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.w	Op2_SkipGoto
	cmp.w	(a2)+,d0
	bgt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_SwitchScrX - a2 op: three-way branch on the object's screen x: left of lo, right of hi, or inside
; In:  a2 = script: words lo, hi, offLeft, offRight
; Out: screen x < lo branches by offLeft, > hi by offRight; inside NEXT after both offsets
; Evidence: both out-of-range paths go through $2D20 (skip a word, branch); executed in the TAS
; ---------------------------------------------------------------------------
Op2_SwitchScrX:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.w	Op2_SkipGoto
	cmp.w	(a2)+,d0
	bgt.w	Op2_SkipGoto
	addq.l	#$4,a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_BranchOffScrY - a2 op: branches when the object's screen y ($42(a6) - g_CameraY) is outside [lo,hi]; also the y half of Op2_BranchOffScrBox
; In:  a2 = script: words lo, hi, offset
; Out: branch by offset, else NEXT after it
; Evidence: Op2_BranchOffScrBox branches here when x is inside; executed in the TAS
; ---------------------------------------------------------------------------
Op2_BranchOffScrY:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a2)+,d0
	blt.s	loc_00089A
	cmp.w	(a2)+,d0
	bgt.w	Op2_Goto
	addq.l	#$2,a2
	movea.w	(a2)+,a0
	jmp	(a0)
loc_00089A:
	addq.l	#$2,a2
	adda.w	(a2),a2
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KillOffscreen50 - a2 op: deletes the object when it is more than 50 px outside the 320x224 screen
; In:  a2 = script (no operand); a6 = object
; Out: Op2_Kill when screen x + 50 >= $1A4 or screen y + 50 >= $144 (unsigned), else NEXT
; Evidence: $1A4 = 320 + 2*50, $144 = 224 + 2*50; executed in the TAS
; ---------------------------------------------------------------------------
Op2_KillOffscreen50:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	addi.w	#$32,d0
	cmpi.w	#$1A4,d0
	bcc.w	Op2_Kill
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	addi.w	#$32,d0
	cmpi.w	#$144,d0
	bcc.w	Op2_Kill
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KillOffscreen100 - a2 op: deletes the object when it is more than 100 px outside the screen (box $208 x $1A8)
; In:  a2 = script (no operand)
; Out: Op2_Kill or NEXT
; Evidence: $208 = 320 + 200, $1A8 = 224 + 200; executed in the TAS
; ---------------------------------------------------------------------------
Op2_KillOffscreen100:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	addi.w	#$64,d0
	cmpi.w	#$208,d0
	bcc.w	Op2_Kill
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	addi.w	#$64,d0
	cmpi.w	#$1A8,d0
	bcc.w	Op2_Kill
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KillOffscreen200 - a2 op: deletes the object when it is more than 200 px outside the screen (box $2D0 x $270)
; In:  a2 = script (no operand)
; Out: Op2_Kill or NEXT
; Evidence: $2D0 = 320 + 400, $270 = 224 + 400; executed in the TAS
; ---------------------------------------------------------------------------
Op2_KillOffscreen200:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	addi.w	#$C8,d0
	cmpi.w	#$2D0,d0
	bcc.w	Op2_Kill
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	addi.w	#$C8,d0
	cmpi.w	#$270,d0
	bcc.w	Op2_Kill
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KillOffScrX - a2 op: deletes the object when its screen x is outside [lo,hi]
; In:  a2 = script: words lo, hi
; Out: Op2_Kill or NEXT
; Evidence: executed in the TAS
; ---------------------------------------------------------------------------
Op2_KillOffScrX:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.s	Op2_Kill
	cmp.w	(a2)+,d0
	bgt.s	Op2_Kill
	movea.w	(a2)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op2_KillOffScrBox - a2 op: deletes the object unless xlo <= screen x <= xhi and ylo <= screen y < yhi
; In:  a2 = script: words xlo, xhi, ylo, yhi
; Out: NEXT when inside, else falls into Op2_Kill
; Evidence: the last test is blt.w $7E0 (strictly below yhi); executed in the TAS
; ---------------------------------------------------------------------------
Op2_KillOffScrBox:
	move.w	$3E(a6),d0
	sub.w	(g_CameraX).w,d0
	cmp.w	(a2)+,d0
	blt.s	Op2_Kill
	cmp.w	(a2)+,d0
	bgt.s	Op2_Kill
; ---------------------------------------------------------------------------
; Op2_KillOffScrY - a2 op: destroys the object unless ylo <= its screen y < yhi (the y half of Op2_KillOffScrBox)
; In:  a2 = script: words ylo, yhi; a6 = object
; Out: NEXT (VM2_Next) when inside, else falls into Op2_Kill
; Evidence: move.w $42(a6),d0 / sub.w g_CameraY / two cmp.w (a2)+,d0; 49,218 computed jumps in the corpus (call-site log)
; ---------------------------------------------------------------------------
Op2_KillOffScrY:
	move.w	$42(a6),d0
	sub.w	(g_CameraY).w,d0
	cmp.w	(a2)+,d0
	blt.s	Op2_Kill
	cmp.w	(a2)+,d0
	blt.w	VM2_Next
; ---------------------------------------------------------------------------
; Op2_Kill - a2 op: deletes the running object (Obj_Destroy) and leaves VM2_RunObject with d0 = -1
; In:  a2 = script (no operand); a6 = object, a3-a5 its blocks
; Out: sp restored from g_VM2SavedSP, rts with d0 = -1
; Evidence: target of the off-screen kill ops $8A2-$93A; executed in the TAS
; ---------------------------------------------------------------------------
Op2_Kill:
	bsr.s	Obj_Destroy
	movea.l	(g_VM2SavedSP).w,a7
	moveq	#-$1,d0
	rts
; ---------------------------------------------------------------------------
; Obj_DestroyOther - native: destroys another object (not the running one): saves a1-a6, makes a0 the current object (a6, a3-a5 from its handles $8/$A/$C), Obj_Destroy, restores
; In:  a0 = object to destroy
; Out: a1-a6 preserved
; Evidence: callers op $24CE (a child handle at a6+word, cleared first); $371C6 sets the registers itself and jumps to the tail $978; executed in the TAS
; ---------------------------------------------------------------------------
Obj_DestroyOther:
	movem.l	a1-a6,-(a7)
	movea.w	a0,a6
	movea.w	$8(a6),a3
	movea.w	$A(a6),a4
	movea.w	$C(a6),a5
; ---------------------------------------------------------------------------
; Obj_DestroyLoaded - Obj_DestroyOther's tail: with a1-a6 already pushed and a6/a3-a5 the object to destroy, Obj_Destroy, then a1-a6 popped back
; In:  a6 = object, a3-a5 its blocks; a1-a6 of the caller on the stack
; Out: a1-a6 restored from the stack
; Evidence: bsr Obj_Destroy / movem.l (a7)+,a1-a6 / rts; ToyChain_KillChild $371C6 pushes a1-a6 and loads the registers itself, then jumps here
; ---------------------------------------------------------------------------
Obj_DestroyLoaded:
	bsr.s	Obj_Destroy
	movem.l	(a7)+,a1-a6
	rts
