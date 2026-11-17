; ---------------------------------------------------------------------------
; VM4_RunObject - object handler of every a4-VM object (installed by the spawn ops through $8A08): a5 = a6 (no child yet) and resumes the object's a4 script at its saved pc $8(a6)
; In:  a6 = a4-VM object (one $5A pool block on the object list)
; Out: falls into VM4_Next; the script's yield/return op returns to Obj_RunList
; Evidence: lea ($53B4).w,a0 before the object alloc $8A08 in the spawn ops $60C4/$613A; ctarget in 151 corpus runs, 98,843 TAS frames (every frame with effects alive)
; ---------------------------------------------------------------------------
VM4_RunObject:
	movea.l	a6,a5
	movea.l	$8(a6),a4
