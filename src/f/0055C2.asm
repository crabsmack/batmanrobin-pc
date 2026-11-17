; ---------------------------------------------------------------------------
; VM4_ResumeA5 - a4 VM: continues the a4 script of the object in a5 from its saved pc $8(a5); the return point of Op4_ReplaceSelf's re-init
; In:  a5 = object (= a6 here)
; Out: NEXT in that object's script
; Evidence: pea (VM4_ResumeA5).l in Op4_ReplaceSelf, entered by the rts of $6156/$6182; 147 corpus runs
; ---------------------------------------------------------------------------
VM4_ResumeA5:
	movea.l	$8(a5),a4
	movea.w	(a4)+,a0
	jmp	(a0)
; ---------------------------------------------------------------------------
; Op4_SpawnRunChild - a4 op: spawns a child object ($613A) and switches to it: a6 = the child and its script runs right away this frame (the parent stops here; it resumes next frame from its own $8)
; In:  a4 = script: word failure offset (0 = end the run), word child-script offset, word parameter-table offset (0 = start at d2-d5)
; Out: NEXT in the child's script (a6 = a5 = child)
; Evidence: bsr $613A / movea.l a5,a6 / movea.l $8(a6),a4; script uses $1AB56, $298D0, $298F0 (child script = the words right after the op, offset 4); 34 corpus runs
; ---------------------------------------------------------------------------
Op4_SpawnRunChild:
	bsr.w	VM4_SpawnChild
	movea.l	a5,a6
	movea.l	$8(a6),a4
	movea.w	(a4)+,a0
	jmp	(a0)
