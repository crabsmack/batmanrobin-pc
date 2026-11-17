; ---------------------------------------------------------------------------
; WeaponDrop_CommandNative - a2 native: hands the weapon-drop director the command script in the following dc.l (spawning the director if there is none)
; In:  a2 = script at the dc.l command script
; Out: a2 past the long; as WeaponDrop_Command (a2-a6 preserved)
; Evidence: op $2CBC pointers at $29292/$29334/$30144/$30162/$3D03C/$3D0FA, with scripts $14036/$14058 (drop) or $13F34 (stop); 10 TAS frames (70456-114869)
; ---------------------------------------------------------------------------
WeaponDrop_CommandNative:
	movea.l	a2,a1
	addq.l	#$4,a2
	movem.l	a2-a6,-(a7)
	bsr.w	WeaponDrop_Command
	movem.l	(a7)+,a2-a6
	rts
