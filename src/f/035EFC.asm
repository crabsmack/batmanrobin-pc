; ---------------------------------------------------------------------------
; Enemy_BombRabbit_TickFuse - fuse tick of a lit bomb rabbit: script var 1 += the inline rate (saturating at $8000), var 2 += var 1, so var 2's top bit (the bomb's blink) toggles faster and faster
; In:  a2 = script: word rate ($140); a4 = the rabbit's a4 block
; Out: vars 1/2 ($22/$24(a4)) updated
; Evidence: `2CB2 007C 0140` at $35E7E every frame of the fuse coroutine; TAS f82646-f115353; f082680 a lit rabbit with vars $2BC0/$1380 shows its bomb orange
; ---------------------------------------------------------------------------
Enemy_BombRabbit_TickFuse:
	move.w	$22(a4),d0
	add.w	(a2)+,d0
	bpl.s	loc_035F08
	move.w	#$8000,d0
loc_035F08:
	move.w	d0,$22(a4)
	add.w	d0,$24(a4)
	rts
; ---------------------------------------------------------------------------
; Enemy_BombRabbit_FuseBlink - anim switch native: d0 = top bit of script var 2, picking which of the two bomb frames ($4718 / $4722) the rabbit's animation draws (the blink driven by Enemy_BombRabbit_TickFuse)
; In:  a4 = the rabbit's a4 block
; Out: d0 = 0 or 1
; Evidence: `2D30 FFxx` cases in the rabbit animation scripts $35F1E-$36012 (each frame draws the rabbit then one bomb frame); runtime entry (ctarget) in the corpus
; ---------------------------------------------------------------------------
Enemy_BombRabbit_FuseBlink:
	move.w	$24(a4),d0
	rol.w	#$1,d0
	andi.w	#$1,d0
	rts
