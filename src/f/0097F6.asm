; ---------------------------------------------------------------------------
; Scroll_NextCmd - fetches the next parallax command word from a5 and dispatches it through the 4-byte jump table that follows (0 = end of list)
; In:  a5 = command list
; Out: returns when it reads command 0; each handler branches back here
; Evidence: jmp $97FC(pc,d0.w): 0 -> rts, 4 ScrollJt_LayerA, $C ScrollJt_CameraA, $14 ScrollJt_CameraB, $18 ScrollJt_CameraOfsB; entries 8/$10/$1C/$20 ($9804/$980C/$9818/$981C -> $9834/$9864/$98C2/$98E0) never ran in the corpus; also called directly by $33398, $475C2
; ---------------------------------------------------------------------------
Scroll_NextCmd:
	move.w	(a5)+,d0
	jmp	ScrollJt_End(pc,d0.w)
; ---------------------------------------------------------------------------
; ScrollJt_End - parallax command 0: the jump-table base itself, an rts that ends the command list
; In:  a5 = after the 0 word
; Out: returns to Scroll_RunParallax/Scroll_NextCmd caller
; Evidence: jmp $97FC(pc,d0.w) with d0 = 0 lands on this rts; every command list (Tbl_00970A etc.) ends with a 0 word; runtime seed $97FC
; ---------------------------------------------------------------------------
ScrollJt_End:
	rts
