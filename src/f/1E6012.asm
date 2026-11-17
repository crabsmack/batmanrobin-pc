; ---------------------------------------------------------------------------
; Snd_PlaySong - starts song d0: looks up its (song-in-module, module offset) pair in the 4-byte table at $1E6028 and starts it in that module through Snd_StartSong
; In:  d0 = song (1-$13; Sound_RestartMusic never passes 0)
; Out: as Snd_StartSongBody
; Evidence: a0 = Tbl_1E7EE4 + word $1E602A[4*d0] (0/8/$10 = bank $1E8000/$1F0000/$1F8000), d0 = word $1E6028[4*d0]; e.g. song 3 = song 5 of module 1, song 1 = song 1 of module 0; only caller Sound_RestartMusic $A1DE; 146 corpus runs
; ---------------------------------------------------------------------------
Snd_PlaySong:
	lea.l	(Tbl_1E7EE4).l,a0
	add.w	d0,d0
	add.w	d0,d0
	adda.w	Tbl_1E602A(pc,d0.w),a0
	move.w	Tbl_1E6028(pc,d0.w),d0
	jmp	Snd_StartSong(pc)
