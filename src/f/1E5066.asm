; ---------------------------------------------------------------------------
; Snd_StartSong - driver API stub: starts song d0 of the music module whose Tbl_1E7EE4 entry is a0 (jsr Snd_StartSongBody), a1 preserved
; In:  a0 = module entry in Tbl_1E7EE4 (long song-pointer list, long module header), d0 = song number in the module
; Out: as Snd_StartSongBody
; Evidence: entered only by Snd_PlaySong (jmp); Tbl_1E7EE4 = $1E8012/$1E8000, $1F0012/$1F0000, $1F8012/$1F8000 (the three 32 KB music banks); 146 corpus runs
; ---------------------------------------------------------------------------
Snd_StartSong:
	move.l	a1,-(a7)
	lea.l	(Snd_StartSongBody).l,a1
	jsr	(a1)
	movea.l	(a7)+,a1
	rts
